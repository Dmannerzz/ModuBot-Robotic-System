#include "obstacle_system.h"
#include "../drivers/ultrasonic.h"
#include "../drivers/scanner.h"

// ==========================
// INTERNAL SENSOR
// ==========================
static Ultrasonic ultrasonic;

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
    ultrasonic.startBackgroundTask(25);

    scanner.begin(&ultrasonic);

    Serial.println("[ObstacleSystem] INIT COMPLETE");
}

// ==========================
// START SCAN (ASYNC ONLY)
// ==========================
void ObstacleSystem::startScan() {

    constexpr unsigned long SCAN_COOLDOWN_MS = 2000;

    if (scanActive) return;
    if (millis() - lastScanTime < SCAN_COOLDOWN_MS) return;

    scanActive = true;
    lastScanTime = millis();

    scanner.startScan();

    Serial.println("SCAN LOCKED");
}

void ObstacleSystem::cancelScan() {
    if (!scanActive) return;

    scanActive = false;
    scanner.cancelScan();
    Serial.println("SCAN RELEASED");
}

// ==========================
// FILTER
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
// DISTANCE
// ==========================
long ObstacleSystem::getDistance() {

    int d = ultrasonic.getFilteredDistance();

    if (d <= 0 || d >= 400) return 0;

    return d;
}

// ==========================
// MAIN UPDATE (FULLY ASYNC)
// ==========================
void ObstacleSystem::update() {

    static unsigned long lastSensorPush = 0;
    static unsigned long lastValidReading = 0;

    int raw = ultrasonic.getFilteredDistance();

    if (!isValidReading(raw)) {

        if (millis() - lastValidReading > 500) {

            if (obstacleState) {
                obstacleState = false;
                eventQueue->push(EventType::OBSTACLE_CLEARED, 0);
            }
        }

        return;
    }

    lastValidReading = millis();

    int distance = smoothDistance(raw);

    lastDistance = distance;

    // sensor stream
    if (millis() - lastSensorPush > 50) {
        lastSensorPush = millis();
        eventQueue->push(EventType::SENSOR_UPDATE, distance);
    }

    // obstacle detected
    if (distance > 0 && distance < OBSTACLE_THRESHOLD) {

        if (!obstacleState) {

            obstacleState = true;

            eventQueue->push(EventType::OBSTACLE_DETECTED, distance);
        }
    }

    else {

        if (obstacleState) {

            obstacleState = false;

            eventQueue->push(EventType::OBSTACLE_CLEARED, distance);
        }
    }

    // ==========================
    // SCANNER BACKGROUND CHECK
    // ==========================
    if (scanActive) {

        scanner.update();

        if (scanner.isScanComplete()) {

            ScanResult result = scanner.getResult();

            int best = result.getBestDirection();

            eventQueue->push(EventType::SCAN_COMPLETE, best);

            scanActive = false;
        }
    }
}