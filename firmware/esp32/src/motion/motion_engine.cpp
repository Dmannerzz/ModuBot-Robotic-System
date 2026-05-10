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
// SENSOR INPUT (DATA ONLY)
// ==========================
void MotionEngine::setDistance(int distance) {
    lastDistance = distance;
}

// ==========================
// FORWARD (PID CONTROLLED)
// ==========================
void MotionEngine::forward(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    float correction = pid.compute(targetYaw, 0);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, -255, 255);
    right = constrain(right, -255, 255);

    Motors::set(left, right);
}

// ==========================
// BACKWARD (PID CONTROLLED)
// ==========================
void MotionEngine::backward(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    float correction = pid.compute(targetYaw, 0);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, -255, 255);
    right = constrain(right, -255, 255);

    Motors::set(-left, -right);
}

// ==========================
// LEFT TURN
// ==========================
void MotionEngine::left(uint16_t speed) {

    speed = constrain(speed, 0, 255);
    Motors::set(-speed, speed);
}

// ==========================
// RIGHT TURN
// ==========================
void MotionEngine::right(uint16_t speed) {

    speed = constrain(speed, 0, 255);
    Motors::set(speed, -speed);
}

// ==========================
// STOP
// ==========================
void MotionEngine::stop() {
    currentSpeed = 0;
    Motors::stop();
}
