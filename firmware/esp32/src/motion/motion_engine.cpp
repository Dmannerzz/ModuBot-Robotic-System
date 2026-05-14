#include "motion_engine.h"
#include "../drivers/motors.h"
#include "../systems/route_storage.h"
#include <Arduino.h>

// ==========================
// INIT
// ==========================
void MotionEngine::begin() {

    pid.begin(2.0, 0.0, 0.5);
    targetYaw = 0;
    lastExecuteTime = millis();

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
// ATTACH STORAGE
// ==========================
void MotionEngine::attachStorage(RouteStorage* storage) {

    storageRef = storage;
    Serial.println("[MotionEngine] Storage ATTACHED");
}

// ==========================
// ENABLE LOGGING
// ==========================
void MotionEngine::enableLogging(bool enabled) {

    loggingEnabled = enabled;
    Serial.print("[MotionEngine] Logging ");
    Serial.println(enabled ? "ENABLED" : "DISABLED");
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

    Serial.println("[MotionEngine] EXECUTE CALLED");

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    uint16_t speed = constrain(cmd.speed, 0, 255);

    // ==========================
    // REAL PID INTEGRATION WITH ACTUAL DT
    // ==========================
    unsigned long now = millis();
    float dt = (now - lastExecuteTime) / 1000.0f;
    lastExecuteTime = now;

    // Safety clamp for dt
    if (dt <= 0.0f || dt > 0.1f) {
        dt = 0.02f;
    }

    float currentYaw = 0;

    if (imuRef != nullptr) {
        imuRef->update();
        currentYaw = imuRef->getYaw();
    }

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

    // ==========================
    // ROUTE LOGGING (IF ENABLED)
    // ==========================
    if (loggingEnabled && storageRef != nullptr) {
        storageRef->logCommand(cmd, currentYaw);
    }
}