#pragma once

#include "event_system.h"

class MotionEngine;
class RouteLogger;
class PatrolSystem;
class ControlPolicy;
class ObstacleSystem;

enum class RobotState {
    IDLE,
    MANUAL,
    OBSTACLE_AVOIDANCE,
    PATROL
};

class StateMachine {
public:

    void init(EventQueue* queue, ObstacleSystem* obstacleSystem);
    void update();

private:

    void handleEvent(const Event& event);

    void handleManual(const Event& event);
    void handleObstacle(const Event& event);
    void handlePatrol(const Event& event);
    void handleIdle(const Event& event);

    void transitionTo(RobotState newState);

private:

    EventQueue* eventQueue = nullptr;

    RobotState currentState = RobotState::IDLE;
    ObstacleSystem* obstacleSystem = nullptr;
};
