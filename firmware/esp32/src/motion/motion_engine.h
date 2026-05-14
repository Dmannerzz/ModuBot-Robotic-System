#pragma once

#include <Arduino.h>
#include "pid_controller.h"
#include "../core/motion_command.h"
#include "../drivers/imu.h"

class RouteLogger;

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

    // ROUTE LOGGING
    void attachLogger(RouteLogger* logger);
    void enableLogging(bool enabled);

private:
    PIDController pid;

    IMU* imuRef = nullptr;
    RouteLogger* loggerRef = nullptr;

    float targetYaw = 0.0f;

    bool safetyOverride = false;
    bool loggingEnabled = false;

    int baseSpeed = 255;  // Max speed (was 180)

    unsigned long lastExecuteTime = 0;

    float normalizeAngle(float angle);
};
