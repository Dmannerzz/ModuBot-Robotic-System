#include "state_machine.h"
#include "motors.h"
#include "motion_engine.h"

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {
    eventQueue = queue;
    currentState = RobotState::IDLE;
}

// ==========================
// MAIN UPDATE LOOP
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

    // ---- HIGH PRIORITY OVERRIDES ----
    if (event.type == EventType::OBSTACLE_DETECTED) {
        transitionTo(RobotState::OBSTACLE_AVOIDANCE);
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
            return;

        default:
            break;
    }

    // ---- STATE HANDLERS ----
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
// STATE BEHAVIORS
// ==========================

void StateMachine::handleManual(const Event& event) {

    switch (event.type) {

        case EventType::MOVE_FORWARD:
            Motors::forward(event.value);
            break;

        case EventType::MOVE_BACKWARD:
            Motors::backward(event.value);
            break;

        case EventType::TURN_LEFT:
            Motors::left(event.value);
            break;

        case EventType::TURN_RIGHT:
            Motors::right(event.value);
            break;

        case EventType::STOP:
            Motors::stop();
            break;

        default:
            break;
    }
}

void StateMachine::handleObstacle(const Event& event) {

    if (event.type == EventType::OBSTACLE_DETECTED) {
        Motors::stop();
        Motors::backward(200);
    }

    if (event.type == EventType::SENSOR_UPDATE) {
        int distance = event.value;

        if (distance < 30) {
            Motors::stop();
            Motors::turnRight(300);
        } else {
            Motors::forward(200);
        }
    }
}

void StateMachine::handlePatrol(const Event& event) {
    if (event.type == EventType::TIMER_TICK) {
        Motors::forward(180);
    }
}

void StateMachine::handleIdle(const Event& event) {
    Motors::stop();
}

// ==========================
// STATE TRANSITION
// ==========================
void StateMachine::transitionTo(RobotState newState) {

    if (currentState == newState) return;

    currentState = newState;

    Serial.print("STATE → ");
    Serial.println((int)newState);
}
