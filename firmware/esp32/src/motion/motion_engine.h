#pragma once
#include <Arduino.h>
#include "pid_controller.h"

class MotionEngine {
public:
    void begin();

    // movement API (speed-based)
    void forward(uint16_t speed = 200);
    void backward(uint16_t speed = 200);
    void left(uint16_t speed = 200);
    void right(uint16_t speed = 200);
    void stop();

    // optional orientation control (MPU integration later)
    void setYaw(float yaw);

    // sensor input (for safety layer)
    void setDistance(int distance);

private:
    PIDController pid;

    float targetYaw = 0;

    int currentSpeed = 0;
    int lastDistance = 100;

    bool safetyOverride = false;
};
