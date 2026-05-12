#include "ultrasonic.h"
#include <Arduino.h>

#define TRIG_PIN 26
#define ECHO_PIN 36

// ==========================
// INIT
// ==========================
void Ultrasonic::begin() {

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(TRIG_PIN, LOW);
}

// ==========================
// READ DISTANCE
// ==========================
int Ultrasonic::readDistance() {

    // trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    // read echo
    long duration = pulseIn(ECHO_PIN, HIGH, 50000);

    // timeout protection
    if (duration <= 0) {
        return lastValidDistance;
    }

    // convert to cm
    int distance = duration * 0.034f / 2.0f;

    // sanity filtering
    if (distance < 2 || distance > 400) {
        return lastValidDistance;
    }

    // smoothing
    distance =
        (lastValidDistance * 0.7f) +
        (distance * 0.3f);

    lastValidDistance = distance;

    return distance;
}
