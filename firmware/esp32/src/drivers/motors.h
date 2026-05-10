#pragma once
#include <Arduino.h>

class Motors {
public:
    static void init();

    // PURE ACTUATOR INTERFACE
    static void set(int leftPWM, int rightPWM);
    static void stop();

private:
    static void applyLeft(int pwm);
    static void applyRight(int pwm);
};
