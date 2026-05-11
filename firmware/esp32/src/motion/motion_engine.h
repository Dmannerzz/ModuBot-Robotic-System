#pragma once

#include <Arduino.h>
#include "pid_controller.h"
#include "motion_command.h"
#include "../drivers/imu.h"

class MotionEngine {
public:
    void begin();

    // CORE EXECUTION INTERFACE
    void execute(const MotionCommand& cmd);

    // HEADING CONTROL
    void setYaw(float yaw);

    // SAFETY CONTROL
    void setSafetyOverride(bool enabled);

    // OPTIONAL: attach IMU reference (dependency injection)
    void attachIMU(IMU* imu);

private:
    PIDController pid;

    IMU* imuRef = nullptr;

    float targetYaw = 0.0f;

    bool safetyOverride = false;

    int baseSpeed = 180;

    float normalizeAngle(float angle);
};
