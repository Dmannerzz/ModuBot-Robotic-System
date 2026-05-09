#include "motion_engine.h"
#include "motors.h"
#include <Arduino.h>

void MotionEngine::begin() {
    pid.begin(2.0, 0.0, 0.5); // tune later
}

void MotionEngine::setYaw(float yaw) {
    targetYaw = yaw;
}

void MotionEngine::forward(int speed) {
    float correction = pid.compute(targetYaw, 0); 
    int left = speed + correction;
    int right = speed - correction;

    left = constrain(left, 0, 255);
    right = constrain(right, 0, 255);

    Motors::set(left, right);
}

void MotionEngine::backward(int speed) {
    Motors::set(-speed, -speed);
}

void MotionEngine::left(int speed) {
    Motors::turnLeft(speed);
}

void MotionEngine::right(int speed) {
    Motors::turnRight(speed);
}

void MotionEngine::stop() {
    Motors::stop();
}
