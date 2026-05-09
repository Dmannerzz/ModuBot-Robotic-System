#include "obstacle_system.h"
#include "ultrasonic.h"

static Ultrasonic ultrasonic;

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

    int raw = ultrasonic.readDistance();

    if (!isValidReading(raw)) return;

    int distance = smoothDistance(raw);

    lastDistance = distance;

    // continuous sensor stream
    eventQueue->push(
        EventType::SENSOR_UPDATE,
        distance
    );

    // ==========================
    // OBSTACLE DETECTION
    // ==========================
    if (distance > 0 && distance < 25) {

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
