#pragma once
#include <Arduino.h>
#include "pid_controller.h"
#include "motion_command.h"
#include "imu.h" 

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

    IMU* imuRef = nullptr;

    float targetYaw = 0;
    bool safetyOverride = false;
};
