#pragma once
#include <Arduino.h>
#include "pid_controller.h"

class MotionEngine {
public:
    void begin();

    // movement API
    void forward(uint16_t speed = 200);
    void backward(uint16_t speed = 200);
    void left(uint16_t speed = 200);
    void right(uint16_t speed = 200);
    void stop();

    // orientation control
    void setYaw(float yaw);

    // SAFETY INPUT (pure signal only)
    void setSafetyOverride(bool enabled);

private:
    PIDController pid;

    float targetYaw = 0;

    bool safetyOverride = false;

    int lastDistance = 100;
};
