#include "route_logger.h"

// ==========================
// INIT
// ==========================
void RouteLogger::begin() {
    clear();
}

// ==========================
// EVENT → MOTION MAPPING
// ==========================
MotionCommand RouteLogger::toMotion(EventType type) {

    switch (type) {

        case EventType::MOVE_FORWARD:
            return MotionCommand::FORWARD;

        case EventType::MOVE_BACKWARD:
            return MotionCommand::BACKWARD;

        case EventType::TURN_LEFT:
            return MotionCommand::LEFT;

        case EventType::TURN_RIGHT:
            return MotionCommand::RIGHT;

        case EventType::STOP:
            return MotionCommand::STOP;

        default:
            return MotionCommand::NONE;
    }
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

    if (lastAction != EventType::NONE &&
        stepCount < MAX_ROUTE_STEPS) {

        route[stepCount++] = {
            toMotion(lastAction),
            millis() - lastTimestamp
        };
    }
}

// ==========================
// LOG EVENT
// ==========================
void RouteLogger::logEvent(EventType type) {

    if (!recording) return;

    if (type == lastAction) return;

    uint32_t now = millis();

    if (lastAction != EventType::NONE &&
        stepCount < MAX_ROUTE_STEPS) {

        route[stepCount++] = {
            toMotion(lastAction),
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
// STATUS
// ==========================
bool RouteLogger::isRecording() {
    return recording;
}
