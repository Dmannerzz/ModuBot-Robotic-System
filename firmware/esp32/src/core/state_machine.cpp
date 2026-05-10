#include "state_machine.h"
#include "motion_engine.h"
#include "route_logger.h"
#include "patrol_system.h"
#include "control_policy.h"

static MotionEngine motion;
static RouteLogger logger;
static ControlPolicy policy;

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {

    eventQueue = queue;

    currentState = RobotState::IDLE;

    motion.begin();
    logger.begin();

    policy.setAuthority(ControlAuthority::NONE);
}

// ==========================
// UPDATE LOOP
// ==========================
void StateMachine::update() {

    Event event;

    while (eventQueue->pop(event)) {

        // GLOBAL POLICY GATE (single entry control point)
        if (!policy.allows(event)) {
            continue;
        }

        handleEvent(event);
    }
}

// ==========================
// EVENT ROUTER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ==========================
    // SAFETY OVERRIDES (HIGHEST PRIORITY)
    // ==========================
    if (event.type == EventType::OBSTACLE_DETECTED) {

        policy.emergencyStop();

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.stop();

        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        transitionTo(RobotState::MANUAL);

        return;
    }

    // ==========================
    // GLOBAL EVENT HANDLING
    // ==========================
    switch (event.type) {

        case EventType::MODE_MANUAL:
            transitionTo(RobotState::MANUAL);
            return;

        case EventType::MODE_OBSTACLE:
            transitionTo(RobotState::OBSTACLE_AVOIDANCE);
            return;

        case EventType::MODE_PATROL:
            transitionTo(RobotState::PATROL);
            return;

        case EventType::MODE_IDLE:
            transitionTo(RobotState::IDLE);
            motion.stop();
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

    switch (event.type) {

        case EventType::MOVE_FORWARD:
            logger.logEvent(EventType::MOVE_FORWARD);
            motion.forward(event.value);
            break;

        case EventType::MOVE_BACKWARD:
            logger.logEvent(EventType::MOVE_BACKWARD);
            motion.backward(event.value);
            break;

        case EventType::TURN_LEFT:
            logger.logEvent(EventType::TURN_LEFT);
            motion.left(event.value);
            break;

        case EventType::TURN_RIGHT:
            logger.logEvent(EventType::TURN_RIGHT);
            motion.right(event.value);
            break;

        case EventType::STOP:
            logger.logEvent(EventType::STOP);
            motion.stop();
            break;

        default:
            break;
    }
}

// ==========================
// OBSTACLE MODE
// ==========================
// IMPORTANT: NO DIRECT SENSOR LOGIC HERE
// Only reacts to system-level events
void StateMachine::handleObstacle(const Event& event) {

    if (event.type == EventType::SENSOR_UPDATE) {
        motion.setDistance(event.value);
    }
}

// ==========================
// PATROL MODE
// ==========================
void StateMachine::handlePatrol(const Event& event) {

    if (event.type == EventType::TIMER_TICK) {
        motion.forward(180);
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

    if (currentState == newState) return;

    currentState = newState;

    // ==========================
    // CONTROL AUTHORITY UPDATE
    // ==========================
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
