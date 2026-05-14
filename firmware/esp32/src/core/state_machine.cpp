#include "state_machine.h"

#include "../motion/motion_engine.h"
#include "../systems/route_storage.h"
#include "../systems/patrol_system.h"
#include "control_policy.h"
#include "motion_command.h"
#include "../drivers/imu.h"

// ==========================
// INTERNAL SYSTEMS
// ==========================
static MotionEngine motion;
static RouteStorage storage;
static PatrolSystem patrol;
static ControlPolicy policy;
static IMU imu;

// ==========================
// ACTIVE COMMAND (SINGLE SOURCE OF TRUTH)
// ==========================
static MotionCommand activeCmd = { MotionAction::STOP, 0 };

// Maneuver state for obstacle avoidance
static bool maneuverActive = false;
static int maneuverPhase = 0; // 0=initial,1=followup
static unsigned long maneuverStartTime = 0;
static unsigned long maneuverDuration = 0;
static MotionCommand maneuverCmd = { MotionAction::STOP, 0 };

constexpr unsigned long TURN_DURATION_MS = 600;
constexpr unsigned long FORWARD_AFTER_TURN_MS = 400;
constexpr unsigned long CENTER_BURST_MS = 500;

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {

    eventQueue = queue;
    currentState = RobotState::IDLE;

    motion.begin();
    storage.begin();

    patrol.begin(&storage, &motion);

    // ==========================
    // ATTACH STORAGE TO MOTION ENGINE
    // ==========================
    motion.attachStorage(&storage);

    policy.setAuthority(ControlAuthority::NONE);

    imu.begin();
    motion.attachIMU(&imu);

    motion.setSafetyOverride(false);

    motion.execute({MotionAction::STOP, 0});

    Serial.println("System Wiring Complete");
}

// ==========================
// UPDATE LOOP
// ==========================
void StateMachine::update() {

    Event event;

    while (eventQueue->pop(event)) {
        handleEvent(event);
    }

    // ==========================
    // OBSTACLE AVOIDANCE MANEUVER PROCESSING
    // ==========================
    if (currentState == RobotState::OBSTACLE_AVOIDANCE && maneuverActive) {
        unsigned long now = millis();
        unsigned long elapsed = now - maneuverStartTime;

        if (elapsed < maneuverDuration) {
            // actively enforce maneuver command
            motion.execute(maneuverCmd);
        } else {
            // Phase transitions
            if (maneuverPhase == 0) {
                // After an initial turn, perform a short forward burst
                if (maneuverCmd.action == MotionAction::LEFT || maneuverCmd.action == MotionAction::RIGHT) {
                    maneuverCmd = { MotionAction::FORWARD, 200 };
                    maneuverDuration = FORWARD_AFTER_TURN_MS;
                    maneuverStartTime = now;
                    maneuverPhase = 1;
                    motion.execute(maneuverCmd);
                } else {
                    // initial was forward burst — finish
                    maneuverActive = false;
                    motion.execute({ MotionAction::FORWARD, 220 });
                }
            } else {
                // completed follow-up phase
                maneuverActive = false;
                motion.execute({ MotionAction::FORWARD, 220 });
            }
        }
    }

    // Patrol runs independently
    patrol.update();
}

// ==========================
// EVENT ROUTER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ==========================
    // SAFETY OVERRIDE (HIGHEST PRIORITY)
    // ==========================
    if (event.type == EventType::OBSTACLE_DETECTED) {

        // In MANUAL mode, just stop and wait for operator input
        if (currentState == RobotState::MANUAL) {
            motion.execute({MotionAction::STOP, 0});
            Serial.println("Manual: obstacle detected - stopped, awaiting input");
            return;
        }

        // For PATROL or other modes, enter avoidance
        policy.emergencyStop();
        patrol.stop();

        motion.setSafetyOverride(true);

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.execute({MotionAction::STOP, 0});
        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        // Clear emergency but remain in OBSTACLE_AVOIDANCE so
        // the avoidance routine can complete its actions
        policy.resetEmergency();
        motion.setSafetyOverride(false);

        Serial.println("Obstacle cleared (avoidance): staying in OBSTACLE_AVOIDANCE");

        motion.execute({MotionAction::STOP, 0});
        return;
    }

    // ==========================
    // MODE SWITCHING
    // ==========================
    switch (event.type) {

        case EventType::MODE_MANUAL:
            patrol.stop();
            transitionTo(RobotState::MANUAL);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::MODE_OBSTACLE:
            patrol.stop();
            transitionTo(RobotState::OBSTACLE_AVOIDANCE);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::MODE_PATROL:
            transitionTo(RobotState::PATROL);
            patrol.start();
            return;

        case EventType::MODE_IDLE:
            patrol.stop();
            transitionTo(RobotState::IDLE);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::SCAN_COMPLETE:

            // Scheduler for timed avoidance maneuvers
            if (currentState == RobotState::OBSTACLE_AVOIDANCE) {
                int dir = event.value - 100;
                Serial.print("SCAN_COMPLETE dir="); Serial.println(dir);

                maneuverActive = true;
                maneuverPhase = 0;
                maneuverStartTime = millis();

                if (dir == -1) {
                    Serial.println("Scheduled maneuver: LEFT");
                    maneuverCmd = { MotionAction::LEFT, 200 };
                    maneuverDuration = TURN_DURATION_MS;
                } else if (dir == 1) {
                    Serial.println("Scheduled maneuver: RIGHT");
                    maneuverCmd = { MotionAction::RIGHT, 200 };
                    maneuverDuration = TURN_DURATION_MS;
                } else {
                    Serial.println("Scheduled maneuver: FORWARD");
                    maneuverCmd = { MotionAction::FORWARD, 200 };
                    maneuverDuration = CENTER_BURST_MS;
                }

                // execute first command immediately
                motion.execute(maneuverCmd);
            }

            return;

        case EventType::LOG_ROUTE:

            if (!storage.isRecording()) {
                storage.startRecording(patrol.getSelectedRoute());
                motion.enableLogging(true);
                Serial.println("Route Recording Started");
            } else {
                storage.stopRecording();
                motion.enableLogging(false);
                Serial.println("Route Recording Stopped");
            }
            return;

        case EventType::SELECT_ROUTE_0:
            patrol.selectRoute(0);
            return;

        case EventType::SELECT_ROUTE_1:
            patrol.selectRoute(1);
            return;

        case EventType::RESET_LOGS:
            storage.clearRoute(patrol.getSelectedRoute());
            Serial.println("Route Cleared");
            return;

        default:
            break;
    }

    // ==========================
    // STATE DISPATCH
    // ==========================
    switch (currentState) {

        case RobotState::MANUAL:
            handleManual(event);
            break;

        case RobotState::OBSTACLE_AVOIDANCE:
            handleObstacle(event);
            break;

        case RobotState::PATROL:
            handlePatrol(event);
            break;

        case RobotState::IDLE:
            handleIdle(event);
            break;
    }
}

// ==========================
// MANUAL MODE (FIXED CLEAN FLOW)
// ==========================
void StateMachine::handleManual(const Event& event) {

    switch (event.type) {

        case EventType::MOVE_FORWARD:
            activeCmd = { MotionAction::FORWARD, (uint16_t)event.value };
            break;

        case EventType::MOVE_BACKWARD:
            activeCmd = { MotionAction::BACKWARD, (uint16_t)event.value };
            break;

        case EventType::TURN_LEFT:
            activeCmd = { MotionAction::LEFT, (uint16_t)event.value };
            break;

        case EventType::TURN_RIGHT:
            activeCmd = { MotionAction::RIGHT, (uint16_t)event.value };
            break;

        case EventType::STOP:
            activeCmd = { MotionAction::STOP, 0 };
            break;

        default:
            return;
    }

    // SINGLE EXECUTION ONLY
    motion.execute(activeCmd);
}

// ==========================
// OBSTACLE MODE
// ==========================
void StateMachine::handleObstacle(const Event& event) {

    if (event.type != EventType::SENSOR_UPDATE) {
        return;
    }

    // If a timed avoidance maneuver is active, ignore sensor updates to prevent
    // the routine from being interrupted by transient sensor values.
    if (maneuverActive) {
        return;
    }

    if (event.value > 0 && event.value < 30) {
        motion.execute({MotionAction::STOP, 0});
    } else {
        motion.execute({MotionAction::FORWARD, 240});  // Max speed forward
    }
}

// ==========================
// PATROL MODE
// ==========================
void StateMachine::handlePatrol(const Event& event) {
    // handled by PatrolSystem
}

// ==========================
// IDLE MODE
// ==========================
void StateMachine::handleIdle(const Event& event) {
    motion.execute({MotionAction::STOP, 0});
}

// ==========================
// TRANSITION
// ==========================
void StateMachine::transitionTo(RobotState newState) {

    currentState = newState;

    switch (newState) {

        case RobotState::MANUAL:
            policy.setAuthority(ControlAuthority::MANUAL);
            break;

        case RobotState::OBSTACLE_AVOIDANCE:
            policy.setAuthority(ControlAuthority::OBSTACLE);
            break;

        case RobotState::PATROL:
            policy.setAuthority(ControlAuthority::PATROL);
            break;

        case RobotState::IDLE:
            policy.setAuthority(ControlAuthority::NONE);
            break;
    }

    Serial.print("STATE → ");
    Serial.println((int)newState);
}