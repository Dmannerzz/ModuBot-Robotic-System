#pragma once
#include <Arduino.h>
#include "event_system.h"
#include "pid_controller.h"

class MotionEngine {
public:
    void begin();

    // High-level movement API
    void forward(uint16_t speed = 200);
    void backward(uint16_t speed = 200);
    void left(uint16_t speed = 200);
    void right(uint16_t speed = 200);
    void stop();

    // Mode behavior control
    void setManualMode(bool enable);
    void setObstacleMode(bool enable);

    // Update loop (called in main loop)
    void update();

    // Sensor-based correction
    void setDistance(int distance);

private:
    PIDController pid;

    bool manualMode = true;
    bool obstacleMode = false;

    int currentSpeed = 0;
    int targetSpeed = 0;

    int lastDistance = 100;

    void applyMotorMix(int left, int right);
    void applyPIDCorrection();
};
