#include "obstacle_system.h"
#include "../drivers/ultrasonic.h"

static Ultrasonic ultrasonic;
static bool bufferInitialized = false;

// ==========================
// CONFIG
// ==========================
constexpr int OBSTACLE_THRESHOLD = 25;

// ==========================
// INIT
// ==========================
void ObstacleSystem::begin(EventQueue* queue) {

    eventQueue = queue;

    ultrasonic.begin();
}

// ==========================
// SIMPLE FILTER
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

    static unsigned long lastSensorPush = 0;
    static unsigned long lastValidReading = 0;

    int raw = ultrasonic.readDistance();

    // ==========================
    // INVALID READING HANDLING
    // ==========================
    if (!isValidReading(raw)) {

        // sensor timeout recovery
        if (millis() - lastValidReading > 500) {

            if (obstacleState) {

                obstacleState = false;

                eventQueue->push(
                    EventType::OBSTACLE_CLEARED,
                    0
                );
            }
        }

        return;
    }

    lastValidReading = millis();

    // ==========================
    // FILTER INITIALIZATION
    // ==========================
    if (!bufferInitialized) {

        for (int i = 0; i < 5; i++) {
            buffer[i] = raw;
        }

        bufferInitialized = true;
    }

    int distance = smoothDistance(raw);

    lastDistance = distance;

    // ==========================
    // SENSOR STREAM (RATE LIMITED)
    // ==========================
    if (millis() - lastSensorPush > 50) {

        lastSensorPush = millis();

        eventQueue->push(
            EventType::SENSOR_UPDATE,
            distance
        );
    }

    // ==========================
    // OBSTACLE DETECTION
    // ==========================
    if (distance > 0 &&
        distance < OBSTACLE_THRESHOLD) {

        if (!obstacleState) {

            obstacleState = true;

            eventQueue->push(
                EventType::OBSTACLE_DETECTED,
                distance
            );
        }

    } else {

        if (obstacleState) {

            obstacleState = false;

            eventQueue->push(
                EventType::OBSTACLE_CLEARED,
                distance
            );
        }
    }
}
