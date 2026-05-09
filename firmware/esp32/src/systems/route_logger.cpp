#include "route_logger.h"

// ==========================
// INIT
// ==========================
void RouteLogger::begin() {
    clear();
}

// ==========================
// START RECORDING
// ==========================
void RouteLogger::startRecording() {

    clear();

    recording = true;

    lastTimestamp = millis();
    lastAction = EventType::NONE;
}

// ==========================
// STOP RECORDING
// ==========================
void RouteLogger::stopRecording() {

    recording = false;

    // finalize last action
    if (lastAction != EventType::NONE &&
        stepCount < MAX_ROUTE_STEPS) {

        route[stepCount++] = {
            lastAction,
            millis() - lastTimestamp
        };
    }
}

// ==========================
// LOG EVENT
// ==========================
void RouteLogger::logEvent(EventType type) {

    if (!recording) return;

    // ignore repeated same movement
    if (type == lastAction) return;

    uint32_t now = millis();

    // finalize previous step
    if (lastAction != EventType::NONE &&
        stepCount < MAX_ROUTE_STEPS) {

        route[stepCount++] = {
            lastAction,
            now - lastTimestamp
        };
    }

    lastAction = type;
    lastTimestamp = now;
}

// ==========================
// GET STEP
// ==========================
RouteStep RouteLogger::getStep(int index) {
    return route[index];
}

// ==========================
// STEP COUNT
// ==========================
int RouteLogger::getStepCount() {
    return stepCount;
}

// ==========================
// CLEAR
// ==========================
void RouteLogger::clear() {

    stepCount = 0;

    lastAction = EventType::NONE;

    lastTimestamp = 0;
}

// ==========================
// RECORDING STATUS
// ==========================
bool RouteLogger::isRecording() {
    return recording;
}
