#include "motors.h"
#include "config.h"

// left motor pins
#define M_A1 16
#define M_A2 17

// right motor pins
#define M_B1 18
#define M_B2 19

#define M_C1 14
#define M_C2 27
#define M_D1 23
#define M_D2 25

void Motors::init() {
    pinMode(M_A1, OUTPUT);
    pinMode(M_A2, OUTPUT);
    pinMode(M_B1, OUTPUT);
    pinMode(M_B2, OUTPUT);
    pinMode(M_C1, OUTPUT);
    pinMode(M_C2, OUTPUT);
    pinMode(M_D1, OUTPUT);
    pinMode(M_D2, OUTPUT);

    stop();
}

// =======================
// RAW LEFT/RIGHT CONTROL
// =======================
void Motors::setLeft(int speed) {
    speed = constrain(speed, 0, 255);
    analogWrite(M_A1, 0);
    analogWrite(M_A2, speed);
    analogWrite(M_B1, speed);
    analogWrite(M_B2, 0);
}

void Motors::setRight(int speed) {
    speed = constrain(speed, 0, 255);
    analogWrite(M_C1, speed);
    analogWrite(M_C2, 0);
    analogWrite(M_D1, speed);
    analogWrite(M_D2, 0);
}

// =======================
// DIRECTIONAL CONTROL
// =======================
void Motors::forward(int speed) {
    speed = constrain(speed, 0, 255);
    setLeft(speed);
    setRight(speed);
}

void Motors::backward(int speed) {
    speed = constrain(speed, 0, 255);

    analogWrite(M_A1, speed);
    analogWrite(M_A2, 0);

    analogWrite(M_B1, 0);
    analogWrite(M_B2, speed);

    analogWrite(M_C1, 0);
    analogWrite(M_C2, speed);

    analogWrite(M_D1, 0);
    analogWrite(M_D2, speed);
}

void Motors::left(int speed) {
    speed = constrain(speed, 0, 255);

    analogWrite(M_A1, speed);
    analogWrite(M_A2, 0);

    analogWrite(M_B1, 0);
    analogWrite(M_B2, speed);

    analogWrite(M_C1, speed);
    analogWrite(M_C2, 0);

    analogWrite(M_D1, speed);
    analogWrite(M_D2, 0);
}

void Motors::right(int speed) {
    speed = constrain(speed, 0, 255);

    analogWrite(M_A1, 0);
    analogWrite(M_A2, speed);

    analogWrite(M_B1, speed);
    analogWrite(M_B2, 0);

    analogWrite(M_C1, 0);
    analogWrite(M_C2, speed);

    analogWrite(M_D1, 0);
    analogWrite(M_D2, speed);
}

// =======================
// STOP (HARD STOP)
// =======================
void Motors::stop() {
    analogWrite(M_A1, 0);
    analogWrite(M_A2, 0);
    analogWrite(M_B1, 0);
    analogWrite(M_B2, 0);
    analogWrite(M_C1, 0);
    analogWrite(M_C2, 0);
    analogWrite(M_D1, 0);
    analogWrite(M_D2, 0);
}
