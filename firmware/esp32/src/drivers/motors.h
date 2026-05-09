#pragma once
#include <Arduino.h>

class Motors {
public:
    static void init();

    static void setLeft(int speed);
    static void setRight(int speed);

    static void forward(int speed);
    static void backward(int speed);
    static void left(int speed);
    static void right(int speed);

    static void stop();

private:
    static void applyLeft(int pwm, bool forward);
    static void applyRight(int pwm, bool forward);
};
