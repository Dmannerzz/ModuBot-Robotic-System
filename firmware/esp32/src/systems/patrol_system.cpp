#include "obstacle_system.h"
#include <Arduino.h>

// pins 
#define TRIG_PIN 26
#define ECHO_PIN 36

// ==========================
// INIT
// ==========================
void ObstacleSystem::begin(EventQueue* queue) {
    eventQueue = queue;

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

// ==========================
// RAW DISTANCE
// ==========================
int ObstacleSystem::readDistance() {

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 50000);

    if (duration <= 0) return 0;

    return duration * 0.034 / 2;
}

// ==========================
// SIMPLE FILTER (moving average)
// ==========================
int ObstacleSystem::smoothDistance(int newReading) {

    buffer[index] = newReading;
    index = (index + 1) % 5;

    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += buffer[i];
    }

    return sum / 5;
}

// ==========================
// VALIDATION
// ==========================
bool ObstacleSystem::isValidReading(int d) {
    return (d > 0 && d < 400);
}

// ==========================
// MAIN UPDATE LOOP
// ==========================
void ObstacleSystem::update() {

    int raw = readDistance();
    if (!isValidReading(raw)) return;

    int distance = smoothDistance(raw);

    lastDistance = distance;

    // Send to motion engine (optional future use)
    eventQueue->push(EventType::SENSOR_UPDATE, distance);

    // ==========================
    // OBSTACLE DETECTION LOGIC
    // ==========================
    if (distance > 0 && distance < 25) {

        if (!obstacleState) {
            obstacleState = true;
            eventQueue->push(EventType::OBSTACLE_DETECTED, distance);
        }

    } else {

        if (obstacleState) {
            obstacleState = false;
            eventQueue->push(EventType::OBSTACLE_CLEARED, distance);
        }
    }
}
