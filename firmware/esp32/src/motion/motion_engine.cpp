#include "motion_engine.h"
#include "../drivers/motors.h"
#include <Arduino.h>

// ==========================
// INIT
// ==========================
void MotionEngine::begin() {

    pid.begin(2.0, 0.0, 0.5);
    targetYaw = 0;

    Serial.println("[MotionEngine] INIT OK");
}

// ==========================
// ATTACH IMU
// ==========================
void MotionEngine::attachIMU(IMU* imu) {

    imuRef = imu;
    Serial.println("[MotionEngine] IMU ATTACHED");
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
        Serial.println("[MotionEngine] SAFETY OVERRIDE ACTIVE");
    }
}

// ==========================
// COMMAND EXECUTOR (CORE)
// ==========================
void MotionEngine::execute(const MotionCommand& cmd) {

    Serial.println("[MotionEngine] EXECUTE CALLED");   // 🔥 PROOF TEST

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

    float dt = 0.02f;
    float correction = pid.compute(targetYaw, currentYaw, dt);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, -255, 255);
    right = constrain(right, -255, 255);

    // ==========================
    // MOTOR EXECUTION
    // ==========================
    switch (cmd.action) {

        case MotionAction::FORWARD:
            Serial.println("[MotionEngine] FORWARD → Motors::set");
            Motors::set(left, right);
            break;

        case MotionAction::BACKWARD:
            Serial.println("[MotionEngine] BACKWARD → Motors::set");
            Motors::set(-left, -right);
            break;

        case MotionAction::LEFT:
            Serial.println("[MotionEngine] LEFT → Motors::set");
            Motors::set(-speed, speed);
            break;

        case MotionAction::RIGHT:
            Serial.println("[MotionEngine] RIGHT → Motors::set");
            Motors::set(speed, -speed);
            break;

        case MotionAction::STOP:
        default:
            Serial.println("[MotionEngine] STOP → Motors::stop");
            Motors::stop();
            break;
    }
}