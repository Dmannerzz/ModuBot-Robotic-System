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
// OPTIONAL YAW SET
// ==========================
void MotionEngine::setYaw(float yaw) {
    targetYaw = yaw;
}

// ==========================
// FORWARD (PID CONTROLLED)
// ==========================
void MotionEngine::forward(int speed) {

    float correction = pid.compute(targetYaw, 0);

    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    Motors::set(left, right);
}

// ==========================
// BACKWARD (NO PID YET - SAFE)
// ==========================
void MotionEngine::backward(int speed) {
    speed = constrain(speed, 0, 255);

    Motors::set(-speed, -speed);
}

// ==========================
// LEFT TURN (SOFT CONTROL)
// ==========================
void MotionEngine::left(int speed) {
    speed = constrain(speed, 0, 255);

    // differential turning instead of hard stop logic
    Motors::set(-speed, speed);
}

// ==========================
// RIGHT TURN
// ==========================
void MotionEngine::right(int speed) {
    speed = constrain(speed, 0, 255);

    Motors::set(speed, -speed);
}

// ==========================
// STOP
// ==========================
void MotionEngine::stop() {
    Motors::stop();
}
