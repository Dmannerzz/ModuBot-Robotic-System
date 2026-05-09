#include "ultrasonic.h"
#include <Arduino.h>

#define TRIG_PIN 26
#define ECHO_PIN 36

void Ultrasonic::begin() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

int Ultrasonic::readDistance() {

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 50000);

    if (duration <= 0) return 0;

    return duration * 0.034 / 2;
}
