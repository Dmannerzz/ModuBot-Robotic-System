#include "state_machine.h"

#include "../motion/motion_engine.h"
#include "../systems/route_storage.h"
#include "../systems/patrol_system.h"
#include "../systems/obstacle_system.h"
#include "control_policy.h"
#include "motion_command.h"
#include "../drivers/imu.h"

// ==========================
// SYSTEMS
// ==========================
static MotionEngine motion;
static RouteStorage storage;
static PatrolSystem patrol;
static ControlPolicy policy;
static IMU imu;

// ==========================
// MOTION STATE
// ==========================
static MotionCommand activeCmd = { MotionAction::STOP, 0 };

// ==========================
// OBSTACLE MANEUVER STATE
// ==========================
static bool maneuverActive = false;
static int maneuverPhase = 0;
static unsigned long maneuverStartTime = 0;
static unsigned long maneuverDuration = 0;
static MotionCommand maneuverCmd = { MotionAction::STOP, 0 };

constexpr unsigned long TURN_DURATION_MS = 600;
constexpr unsigned long FORWARD_AFTER_TURN_MS = 400;
constexpr unsigned long CENTER_BURST_MS = 500;

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue, ObstacleSystem* obstacleSys) {

    eventQueue = queue;
    obstacleSystem = obstacleSys;
    currentState = RobotState::IDLE;

    motion.begin();
    storage.begin();

    patrol.begin(&storage, &motion);
    motion.attachStorage(&storage);

    policy.setAuthority(ControlAuthority::NONE);

    imu.begin();
    motion.attachIMU(&imu);

    motion.setSafetyOverride(false);
    motion.execute({MotionAction::STOP, 0});

    obstacleSystem->begin(queue);

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
    // MANEUVER EXECUTION (ASYNC SAFE)
    // ==========================
    if (maneuverActive) {

        if (millis() - maneuverStartTime < maneuverDuration) {
            motion.execute(maneuverCmd);
        } else {
            maneuverActive = false;

            motion.execute({MotionAction::FORWARD, 220});
        }
    }

    if (obstacleSystem != nullptr) {
        obstacleSystem->update();
    }
    patrol.update();
}

// ==========================
// EVENT HANDLER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ==========================
    // OBSTACLE DETECTED
    // ==========================
    if (event.type == EventType::OBSTACLE_DETECTED) {

        if (currentState == RobotState::MANUAL) {
            motion.execute({MotionAction::STOP, 0});
            Serial.println("Manual stop due to obstacle");
            return;
        }

        policy.emergencyStop();
        patrol.stop();

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.setSafetyOverride(true);
        motion.execute({MotionAction::STOP, 0});

        // 🔥 ONLY TRIGGER SCAN (ASYNC)
        if (obstacleSystem != nullptr) {
            obstacleSystem->startScan();
        }

        return;
    }

    // ==========================
    // SCAN RESULT (CORE FIX)
    // ==========================
    if (event.type == EventType::SCAN_COMPLETE) {

        if (currentState != RobotState::OBSTACLE_AVOIDANCE) {
            Serial.println("SCAN_COMPLETE ignored outside obstacle mode");
            return;
        }

        int dir = event.value;

        policy.resetEmergency();

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.setSafetyOverride(true);

        maneuverActive = true;
        maneuverPhase = 0;
        maneuverStartTime = millis();

        if (dir == 0) {
            Serial.println("SCAN → LEFT");
            maneuverCmd = { MotionAction::LEFT, 255 };
            maneuverDuration = TURN_DURATION_MS;
        }
        else if (dir == 2) {
            Serial.println("SCAN → RIGHT");
            maneuverCmd = { MotionAction::RIGHT, 255 };
            maneuverDuration = TURN_DURATION_MS;
        }
        else if (dir == 1) {
            Serial.println("SCAN → FORWARD");
            maneuverCmd = { MotionAction::FORWARD, 255 };
            maneuverDuration = CENTER_BURST_MS;
        }
        else {
            Serial.println("SCAN → BLOCKED → RIGHT");
            maneuverCmd = { MotionAction::RIGHT, 255 };
            maneuverDuration = TURN_DURATION_MS;
        }

        motion.execute(maneuverCmd);
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
// MANUAL MODE
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

    motion.execute(activeCmd);
}

// ==========================
// OBSTACLE MODE
// ==========================
void StateMachine::handleObstacle(const Event& event) {

    if (event.type != EventType::SENSOR_UPDATE) return;

    if (maneuverActive) return;

    if (event.value > 0 && event.value < 30) {
        motion.execute({MotionAction::STOP, 0});
    } else {
        motion.execute({MotionAction::FORWARD, 255});
    }
}

// ==========================
// PATROL MODE
// ==========================
void StateMachine::handlePatrol(const Event& event) {
    // handled by PatrolSystem
}

// ==========================
// IDLE
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
            motion.setSafetyOverride(false);
            maneuverActive = false;
            break;

        case RobotState::OBSTACLE_AVOIDANCE:
            policy.setAuthority(ControlAuthority::OBSTACLE);
            break;

        case RobotState::PATROL:
            if (obstacleSystem != nullptr) {
                obstacleSystem->cancelScan();
            }
            policy.setAuthority(ControlAuthority::PATROL);
            motion.setSafetyOverride(false);
            maneuverActive = false;
            break;

        case RobotState::IDLE:
            if (obstacleSystem != nullptr) {
                obstacleSystem->cancelScan();
            }
            policy.setAuthority(ControlAuthority::NONE);
            motion.setSafetyOverride(false);
            maneuverActive = false;
            break;
    }

    if (newState == RobotState::MANUAL && obstacleSystem != nullptr) {
        obstacleSystem->cancelScan();
    }

    Serial.print("STATE → ");
    Serial.println((int)newState);
}