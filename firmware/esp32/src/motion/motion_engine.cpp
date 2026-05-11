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
// FORWARD
// ==========================
void MotionEngine::forward(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    speed = constrain(speed, 0, 255);

    int correction = 0;

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    Motors::set(left, right);
}

// ==========================
// BACKWARD
// ==========================
void MotionEngine::backward(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    speed = constrain(speed, 0, 255);

    int correction = 0;

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    Motors::set(-left, -right);
}

// ==========================
// LEFT TURN
// ==========================
void MotionEngine::left(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    speed = constrain(speed, 0, 255);

    Motors::set(-speed, speed);
}

// ==========================
// RIGHT TURN
// ==========================
void MotionEngine::right(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    speed = constrain(speed, 0, 255);

    Motors::set(speed, -speed);
}

// ==========================
// STOP
// ==========================
void MotionEngine::stop() {

    Motors::stop();
}
