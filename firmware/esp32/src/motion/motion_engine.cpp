#include "motion_engine.h"
#include "motors.h"
#include <Arduino.h>

// ==========================
// INIT
// ==========================
void MotionEngine::begin() {
    pid.begin(2.0, 0.0, 0.5);
    targetYaw = 0;
}

// ==========================
// ATTACH IMU
// ==========================
void MotionEngine::attachIMU(IMU* imu) {
    imuRef = imu;
}

// ==========================
// YAW CONTROL
// ==========================
void MotionEngine::setYaw(float yaw) {
    targetYaw = yaw;
}

// ==========================
// SAFETY CONTROL
// ==========================
void MotionEngine::setSafetyOverride(bool enabled) {
    safetyOverride = enabled;

    if (enabled) {
        Motors::stop();
    }
}

// ==========================
// COMMAND EXECUTOR (CORE)
// ==========================
void MotionEngine::execute(const MotionCommand& cmd) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    uint16_t speed = constrain(cmd.speed, 0, 255);

    // ==========================
    // REAL PID INTEGRATION
    // ==========================
    float currentYaw = 0;

    if (imuRef != nullptr) {
        imuRef->update();
        currentYaw = imuRef->getYaw();
    }

    float correction = pid.compute(targetYaw, currentYaw);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, -255, 255);
    right = constrain(right, -255, 255);

    // ==========================
    // MOTOR EXECUTION
    // ==========================
    switch (cmd.action) {

        case MotionAction::FORWARD:
            Motors::set(left, right);
            break;

        case MotionAction::BACKWARD:
            Motors::set(-left, -right);
            break;

        case MotionAction::LEFT:
            Motors::set(-speed, speed);
            break;

        case MotionAction::RIGHT:
            Motors::set(speed, -speed);
            break;

        case MotionAction::STOP:
        default:
            Motors::stop();
            break;
    }
}
