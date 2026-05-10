#include "motors.h"

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

// ==========================
// INIT
// ==========================
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

// ==========================
// PURE LEFT MOTOR CONTROL
// ==========================
void Motors::applyLeft(int pwm) {

    pwm = constrain(pwm, -255, 255);

    // deadband protection
    if (abs(pwm) < 20) {
        pwm = 0;
    }

    // forward
    if (pwm >= 0) {

        analogWrite(M_A1, 0);
        analogWrite(M_A2, pwm);

        analogWrite(M_B1, pwm);
        analogWrite(M_B2, 0);
    }

    // reverse
    else {

        pwm = -pwm;

        analogWrite(M_A1, pwm);
        analogWrite(M_A2, 0);

        analogWrite(M_B1, 0);
        analogWrite(M_B2, pwm);
    }
}

// ==========================
// PURE RIGHT MOTOR CONTROL
// ==========================
void Motors::applyRight(int pwm) {

    pwm = constrain(pwm, -255, 255);

    // deadband protection
    if (abs(pwm) < 20) {
        pwm = 0;
    }

    // forward
    if (pwm >= 0) {

        analogWrite(M_C1, pwm);
        analogWrite(M_C2, 0);

        analogWrite(M_D1, pwm);
        analogWrite(M_D2, 0);
    }

    // reverse
    else {

        pwm = -pwm;

        analogWrite(M_C1, 0);
        analogWrite(M_C2, pwm);

        analogWrite(M_D1, 0);
        analogWrite(M_D2, pwm);
    }
}

// ==========================
// MAIN ACTUATOR ENTRY POINT
// ==========================
void Motors::set(int leftPWM, int rightPWM) {

    applyLeft(leftPWM);

    applyRight(rightPWM);
}

// ==========================
// STOP
// ==========================
void Motors::stop() {

    set(0, 0);
}
