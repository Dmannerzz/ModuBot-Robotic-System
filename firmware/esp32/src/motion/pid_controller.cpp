#include "pid_controller.h"

void PIDController::begin(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
    prevError = 0;
    integral = 0;
}

float PIDController::compute(float target, float current) {
    float error = target - current;

    integral += error;
    float derivative = error - prevError;

    prevError = error;

    return (kp * error) + (ki * integral) + (kd * derivative);
}

void PIDController::reset() {
    prevError = 0;
    integral = 0;
}
