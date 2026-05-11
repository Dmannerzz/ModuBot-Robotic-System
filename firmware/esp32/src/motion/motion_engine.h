#pragma once
#include <Arduino.h>
#include "pid_controller.h"
#include "motion_command.h"

class MotionEngine {
public:
    void begin();

    // SINGLE ENTRY POINT NOW
    void execute(const MotionCommand& cmd);

    // orientation control
    void setYaw(float yaw);

    // safety control
    void setSafetyOverride(bool enabled);

private:
    PIDController pid;

    float targetYaw = 0;
    bool safetyOverride = false;
};
