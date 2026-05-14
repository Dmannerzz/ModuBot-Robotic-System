#include "state_machine.h"

#include "../motion/motion_engine.h"
#include "../systems/route_logger.h"
#include "../systems/patrol_system.h"
#include "control_policy.h"
#include "motion_command.h"
#include "../drivers/imu.h"

// ==========================
// INTERNAL SYSTEMS
// ==========================
static MotionEngine motion;
static RouteLogger logger;
static PatrolSystem patrol;
static ControlPolicy policy;
static IMU imu;

// ==========================
// ACTIVE COMMAND (SINGLE SOURCE OF TRUTH)
// ==========================
static MotionCommand activeCmd = { MotionAction::STOP, 0 };

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {

    eventQueue = queue;
    currentState = RobotState::IDLE;

    motion.begin();
    logger.begin();

    patrol.begin(&logger, &motion);

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

        policy.emergencyStop();
        patrol.stop();

        motion.setSafetyOverride(true);

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.execute({MotionAction::STOP, 0});
        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        policy.resetEmergency();
        motion.setSafetyOverride(false);

        transitionTo(RobotState::MANUAL);

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

        case EventType::LOG_ROUTE:

            if (!logger.isRecording()) {
                logger.startRecording();
                Serial.println("Route Recording Started");
            } else {
                logger.stopRecording();
                Serial.println("Route Recording Stopped");
            }
            return;

        case EventType::RESET_LOGS:
            logger.clear();
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

    if (event.value > 0 && event.value < 30) {
        motion.execute({MotionAction::STOP, 0});
    } else {
        motion.execute({MotionAction::FORWARD, 150});
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