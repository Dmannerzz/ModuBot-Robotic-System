#include "globals.h"
#include <Arduino.h>

// ========== ULTRASONIC SENSOR ==========
long readDistanceCM() {

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0) return -1; // no echo

    return duration * 0.034 / 2;
}

// ========== OBSTACLE UPDATE ==========
void updateObstacleSensor() {

    long distance = readDistanceCM();

    // safety filter
    if (distance < 0) {
        obstacleActive = false;
        return;
    }

    obstacleActive = (distance > 0 && distance < 20);
}
