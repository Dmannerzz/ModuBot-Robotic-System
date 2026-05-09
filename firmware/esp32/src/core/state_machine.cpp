#include "state_machine.h"
#include "motors.h"
#include "motion_engine.h"

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

    // ---- EMERGENCY OVERRIDES ----
    if (event.type == EventType::OBSTACLE_DETECTED) {
        transitionTo(RobotState::OBSTACLE_AVOIDANCE);
        Motors::stop();
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
            Motors::stop();
            return;

        default:
            break;
    }

    // ---- STATE DISPATCH ----
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
            Motors::forward();   // continuous
            break;

        case EventType::MOVE_BACKWARD:
            Motors::backward();
            break;

        case EventType::TURN_LEFT:
            Motors::left();
            break;

        case EventType::TURN_RIGHT:
            Motors::right();
            break;

        case EventType::STOP:
            Motors::stop();
            break;

        default:
            break;
    }
}

// ==========================
// OBSTACLE MODE
// ==========================
void StateMachine::handleObstacle(const Event& event) {

    switch (event.type) {

        case EventType::MOVE_FORWARD:
            Motors::forward(200);
            break;

        case EventType::MOVE_BACKWARD:
            Motors::backward(200);
            break;

        case EventType::TURN_LEFT:
            Motors::left(300);
            break;

        case EventType::TURN_RIGHT:
            Motors::right(300);
            break;

        case EventType::SENSOR_UPDATE: {
            int distance = event.value;

            if (distance < 30 && distance > 0) {
                Motors::stop();
                Motors::right(300);
            } else {
                Motors::forward(150);
            }
            break;
        }

        default:
            break;
    }
}

// ==========================
// PATROL MODE
// ==========================
void StateMachine::handlePatrol(const Event& event) {

    if (event.type == EventType::TIMER_TICK) {
        Motors::forward(180);
    }

    if (event.type == EventType::MOVE_FORWARD) {
        Motors::forward(180);
    }

    if (event.type == EventType::TURN_LEFT) {
        Motors::left(200);
    }

    if (event.type == EventType::TURN_RIGHT) {
        Motors::right(200);
    }

    if (event.type == EventType::STOP) {
        Motors::stop();
    }
}

// ==========================
// IDLE
// ==========================
void StateMachine::handleIdle(const Event& event) {
    Motors::stop();
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
