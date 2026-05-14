#include "motors.h"
#include <Arduino.h>

// ==========================
// MOTOR PINS
// ==========================
#define M_A1 16
#define M_A2 17

#define M_B1 18
#define M_B2 19

#define M_C1 14
#define M_C2 27

#define M_D1 23
#define M_D2 25

// ==========================
// PWM CONFIG (ESP32 LEDC)
// ==========================
#define PWM_FREQ 20000
#define PWM_RES 8  // 0 - 255

#define CH_A1 0
#define CH_A2 1
#define CH_B1 2
#define CH_B2 3
#define CH_C1 4
#define CH_C2 5
#define CH_D1 6
#define CH_D2 7

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

    ledcSetup(CH_A1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_A2, PWM_FREQ, PWM_RES);
    ledcSetup(CH_B1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_B2, PWM_FREQ, PWM_RES);
    ledcSetup(CH_C1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_C2, PWM_FREQ, PWM_RES);
    ledcSetup(CH_D1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_D2, PWM_FREQ, PWM_RES);

    ledcAttachPin(M_A1, CH_A1);
    ledcAttachPin(M_A2, CH_A2);
    ledcAttachPin(M_B1, CH_B1);
    ledcAttachPin(M_B2, CH_B2);
    ledcAttachPin(M_C1, CH_C1);
    ledcAttachPin(M_C2, CH_C2);
    ledcAttachPin(M_D1, CH_D1);
    ledcAttachPin(M_D2, CH_D2);

    stop();
}

// ==========================
// LEFT MOTOR
// ==========================
void Motors::applyLeft(int pwm) {

    pwm = constrain(pwm, -255, 255);

    if (abs(pwm) < 20) pwm = 0;

    if (pwm >= 0) {
        ledcWrite(CH_A1, 0);
        ledcWrite(CH_A2, pwm);

        ledcWrite(CH_B1, pwm);
        ledcWrite(CH_B2, 0);
    } else {
        pwm = -pwm;

        ledcWrite(CH_A1, pwm);
        ledcWrite(CH_A2, 0);

        ledcWrite(CH_B1, 0);
        ledcWrite(CH_B2, pwm);
    }
}

// ==========================
// RIGHT MOTOR
// ==========================
void Motors::applyRight(int pwm) {

    pwm = constrain(pwm, -255, 255);

    if (abs(pwm) < 20) pwm = 0;

    if (pwm >= 0) {
        ledcWrite(CH_C1, pwm);
        ledcWrite(CH_C2, 0);

        ledcWrite(CH_D1, pwm);
        ledcWrite(CH_D2, 0);
    } else {
        pwm = -pwm;

        ledcWrite(CH_C1, 0);
        ledcWrite(CH_C2, pwm);

        ledcWrite(CH_D1, 0);
        ledcWrite(CH_D2, pwm);
    }
}

// ==========================
// MAIN ENTRY
// ==========================
void Motors::set(int leftPWM, int rightPWM) {

    applyLeft(leftPWM);
    applyRight(rightPWM);
}

// ==========================
// STOP
// ==========================
void Motors::stop() {

    ledcWrite(CH_A1, 0);
    ledcWrite(CH_A2, 0);

    ledcWrite(CH_B1, 0);
    ledcWrite(CH_B2, 0);

    ledcWrite(CH_C1, 0);
    ledcWrite(CH_C2, 0);

    ledcWrite(CH_D1, 0);
    ledcWrite(CH_D2, 0);
}