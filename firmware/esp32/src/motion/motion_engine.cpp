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
// SENSOR INPUT (SAFETY HOOK)
// ==========================
void MotionEngine::setDistance(int distance) {
    lastDistance = distance;

    if (distance > 0 && distance < 25) {
        safetyOverride = true;
        stop();
    } else {
        safetyOverride = false;
    }
}

// ==========================
// FORWARD (PID + SAFETY)
// ==========================
void MotionEngine::forward(uint16_t speed) {

    if (safetyOverride) {
        Motors::stop();
        return;
    }

    float correction = pid.compute(targetYaw, 0);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    Motors::set(left, right);
}

// ==========================
// BACKWARD (NOW PID CONTROLLED)
// ==========================
void MotionEngine::backward(uint16_t speed) {

    if (safetyOverride) {
        Motors::set(-120, -120);
        return;
    }

    // SAME PID LOGIC AS FORWARD
    float correction = pid.compute(targetYaw, 0);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    // INVERT MOTOR DIRECTION ONLY
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
    currentSpeed = 0;
    Motors::stop();
}
