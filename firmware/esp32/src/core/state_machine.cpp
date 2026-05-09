#include "state_machine.h"
#include "motion_engine.h"

// external motion engine (single source of truth)
static MotionEngine motion;

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {
    eventQueue = queue;
    currentState = RobotState::IDLE;

    motion.begin();
}

// ==========================
// UPDATE LOOP
// ==========================
void StateMachine::update() {

    Event event;

    while (eventQueue->pop(event)) {
        handleEvent(event);
    }
}

// ==========================
// EVENT ROUTER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ---- SAFETY OVERRIDES ----
    if (event.type == EventType::OBSTACLE_DETECTED) {
        transitionTo(RobotState::OBSTACLE_AVOIDANCE);
        motion.stop();
        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        transitionTo(RobotState::MANUAL);
        return;
    }

    // ---- MODE SWITCHING ----
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

        default:
            break;
    }

    // ---- STATE EXECUTION ----
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
            motion.forward(event.value);
            break;

        case EventType::MOVE_BACKWARD:
            motion.backward(event.value);
            break;

        case EventType::TURN_LEFT:
            motion.left(event.value);
            break;

        case EventType::TURN_RIGHT:
            motion.right(event.value);
            break;

        case EventType::STOP:
            motion.stop();
            break;

        default:
            break;
    }
}

// ==========================
// OBSTACLE MODE
// ==========================
void StateMachine::handleObstacle(const Event& event) {

    if (event.type == EventType::SENSOR_UPDATE) {

        int distance = event.value;
        motion.setDistance(distance);

        if (distance < 30 && distance > 0) {
            motion.stop();
            motion.right(250);
        } else {
            motion.forward(150);
        }
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
// IDLE
// ==========================
void StateMachine::handleIdle(const Event& event) {
    motion.stop();
}

// ==========================
// TRANSITION
// ==========================
void StateMachine::transitionTo(RobotState newState) {

    if (currentState == newState) return;

    currentState = newState;

    Serial.print("STATE → ");
    Serial.println((int)newState);
}
