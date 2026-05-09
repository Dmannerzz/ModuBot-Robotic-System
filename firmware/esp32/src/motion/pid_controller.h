#pragma once

class PIDController {
public:
    void begin(float kp, float ki, float kd);

    float compute(float target, float current);
    void reset();

private:
    float kp = 0;
    float ki = 0;
    float kd = 0;

    float prevError = 0;
    float integral = 0;
};
