#include "obstacle_system.h"
#include "../drivers/ultrasonic.h"
#include "../drivers/scanner.h"

static Ultrasonic ultrasonic;
static Scanner scanner;
static bool bufferInitialized = false;
static unsigned long lastScanTime = 0;
static const unsigned long SUPPRESS_CLEAR_MS = 1200;

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
    scanner.begin();
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
// PERFORM SCAN AND GET BEST DIRECTION
// ==========================
int ObstacleSystem::scanAndAvoid() {
    ScanResult result = scanner.performScan();
    int bestDirection = result.getBestDirection();
    
    Serial.print("Best direction: ");
    Serial.println(bestDirection == -1 ? "LEFT" : (bestDirection == 0 ? "FORWARD" : "RIGHT"));
    
    return bestDirection;
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
    // OBSTACLE DETECTION WITH SCANNING
    // ==========================
    if (distance > 0 &&
        distance < OBSTACLE_THRESHOLD) {

        if (!obstacleState) {

            obstacleState = true;
            
            // Trigger scan and get best direction
            int bestDirection = scanAndAvoid();

            eventQueue->push(
                EventType::OBSTACLE_DETECTED,
                distance
            );
            
            // Push scan result (encoded as: -1=LEFT, 0=CENTER, 1=RIGHT, +100 to ensure positive)
            eventQueue->push(
                EventType::SCAN_COMPLETE,
                bestDirection + 100
            );

            // record scan time to suppress transient clears
            lastScanTime = millis();
        }

    } else {

        if (obstacleState) {

            // If we recently scanned, suppress transient clear events
            if (millis() - lastScanTime < SUPPRESS_CLEAR_MS) {
                Serial.println("Obstacle clear suppressed (recent scan)");
                return;
            }

            obstacleState = false;

            eventQueue->push(
                EventType::OBSTACLE_CLEARED,
                distance
            );
        }
    }
}

