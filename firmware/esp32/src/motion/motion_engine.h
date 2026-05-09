#pragma once
#include "pid_controller.h"

class MotionEngine {
public:
    void begin();

    void forward(int speed);
    void backward(int speed);
    void left(int speed);
    void right(int speed);
    void stop();

    void setYaw(float yaw);

private:
    PIDController pid;
    float targetYaw = 0;
};
