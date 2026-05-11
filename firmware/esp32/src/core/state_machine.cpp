#include "state_machine.h"
#include "../motion/motion_engine.h"
#include "../systems/route_logger.h"
#include "../systems/patrol_system.h"
#include "control_policy.h"
#include "motion_command.h"

static MotionEngine motion;
static RouteLogger logger;
static PatrolSystem patrol;
static ControlPolicy policy;

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
}

// ==========================
// UPDATE LOOP
// ==========================
void StateMachine::update() {

    Event event;

    while (eventQueue->pop(event)) {

        handleEvent(event);
    }

    // keep patrol replay alive
    patrol.update();
}

// ==========================
// EVENT ROUTER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ==========================
    // SAFETY OVERRIDES
    // ==========================
    if (event.type == EventType::OBSTACLE_DETECTED) {

        policy.emergencyStop();

        patrol.stop();

        motion.setSafetyOverride(true);

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.stop();

        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        policy.resetEmergency();

        motion.setSafetyOverride(false);

        transitionTo(RobotState::MANUAL);

        return;
    }

    // ==========================
    // GLOBAL EVENT HANDLING
    // ==========================
    switch (event.type) {

        // ==========================
        // MANUAL MODE
        // ==========================
        case EventType::MODE_MANUAL:

            patrol.stop();

            transitionTo(RobotState::MANUAL);

            return;

        // ==========================
        // OBSTACLE MODE
        // ==========================
        case EventType::MODE_OBSTACLE:

            patrol.stop();

            transitionTo(RobotState::OBSTACLE_AVOIDANCE);

            return;

        // ==========================
        // PATROL MODE
        // ==========================
        case EventType::MODE_PATROL:

            transitionTo(RobotState::PATROL);

            patrol.start();

            return;

        // ==========================
        // IDLE MODE
        // ==========================
        case EventType::MODE_IDLE:

            patrol.stop();

            transitionTo(RobotState::IDLE);

            motion.stop();

            return;

        // ==========================
        // ROUTE RECORDING
        // ==========================
        case EventType::LOG_ROUTE:

            if (!logger.isRecording()) {

                logger.startRecording();

                Serial.println("Route Recording Started");

            } else {

                logger.stopRecording();

                Serial.println("Route Recording Stopped");
            }

            return;

        // ==========================
        // CLEAR ROUTE
        // ==========================
        case EventType::RESET_LOGS:

            logger.clear();

            Serial.println("Route Cleared");

            return;

        default:
            break;
    }

    // ==========================
    // STATE EXECUTION
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

    MotionCommand cmd;

    switch (event.type) {

        case EventType::MOVE_FORWARD:

            cmd = {
                MotionAction::FORWARD,
                (uint16_t)event.value
            };

            break;

        case EventType::MOVE_BACKWARD:

            cmd = {
                MotionAction::BACKWARD,
                (uint16_t)event.value
            };

            break;

        case EventType::TURN_LEFT:

            cmd = {
                MotionAction::LEFT,
                (uint16_t)event.value
            };

            break;

        case EventType::TURN_RIGHT:

            cmd = {
                MotionAction::RIGHT,
                (uint16_t)event.value
            };

            break;

        case EventType::STOP:

            cmd = {
                MotionAction::STOP,
                0
            };

            break;

        default:
            return;
    }

    // unified execution path
    motion.execute(cmd);

    // unified logging path
    logger.logCommand(cmd);
}

// ==========================
// OBSTACLE MODE
// ==========================
// perception-only state
void StateMachine::handleObstacle(const Event& event) {

    if (event.type != EventType::SENSOR_UPDATE) {
        return;
    }

    motion.setDistance(event.value);

    MotionCommand cmd;

    if (event.value > 0 && event.value < 30) {

        cmd = {
            MotionAction::STOP,
            0
        };

    } else {

        cmd = {
            MotionAction::FORWARD,
            150
        };
    }

    motion.execute(cmd);
}

// ==========================
// PATROL MODE
// ==========================
void StateMachine::handlePatrol(const Event& event) {

    switch (event.type) {

        case EventType::MOVE_FORWARD:
        case EventType::MOVE_BACKWARD:
        case EventType::TURN_LEFT:
        case EventType::TURN_RIGHT:
        case EventType::STOP:
            return;

        default:
            break;
    }
}

// ==========================
// IDLE MODE
// ==========================
void StateMachine::handleIdle(const Event& event) {

    motion.stop();
}

// ==========================
// STATE TRANSITIONS
// ==========================
void StateMachine::transitionTo(RobotState newState) {

    if (currentState == newState) {
        return;
    }

    currentState = newState;

    // ==========================
    // CONTROL AUTHORITY
    // ==========================
    switch (newState) {

        case RobotState::MANUAL:

            policy.setAuthority(
                ControlAuthority::MANUAL
            );

            break;

        case RobotState::OBSTACLE_AVOIDANCE:

            policy.setAuthority(
                ControlAuthority::OBSTACLE
            );

            break;

        case RobotState::PATROL:

            policy.setAuthority(
                ControlAuthority::PATROL
            );

            break;

        case RobotState::IDLE:

            policy.setAuthority(
                ControlAuthority::NONE
            );

            break;
    }

    Serial.print("STATE → ");

    Serial.println((int)newState);
}
