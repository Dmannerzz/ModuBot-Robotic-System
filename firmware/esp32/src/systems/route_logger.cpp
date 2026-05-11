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

    lastCmd = { MotionAction::STOP, 0 };
}

// ==========================
// STOP RECORDING
// ==========================
void RouteLogger::stopRecording() {

    recording = false;

    // finalize last command
    if (stepCount < MAX_ROUTE_STEPS &&
        lastCmd.action != MotionAction::STOP) {

        route[stepCount++] = {
            lastCmd,
            millis() - lastTimestamp
        };
    }
}

// ==========================
// LOG COMMAND (CORE CHANGE)
// ==========================
void RouteLogger::logCommand(const MotionCommand& cmd) {

    if (!recording) return;

    uint32_t now = millis();

    // ignore duplicates (same motion state)
    if (cmd.action == lastCmd.action &&
        cmd.speed == lastCmd.speed) {
        return;
    }

    // finalize previous step
    if (stepCount < MAX_ROUTE_STEPS &&
        lastCmd.action != MotionAction::STOP) {

        route[stepCount++] = {
            lastCmd,
            now - lastTimestamp
        };
    }

    lastCmd = cmd;
    lastTimestamp = now;
}

// ==========================
// GET STEP
// ==========================
RouteStep RouteLogger::getStep(int index) {
    return route[index];
}

// ==========================
// COUNT
// ==========================
int RouteLogger::getStepCount() {
    return stepCount;
}

// ==========================
// CLEAR
// ==========================
void RouteLogger::clear() {

    stepCount = 0;

    lastCmd = { MotionAction::STOP, 0 };

    lastTimestamp = 0;
}

// ==========================
// STATUS
// ==========================
bool RouteLogger::isRecording() {
    return recording;
}
