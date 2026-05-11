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

    // finalize last step safely
    if (stepCount < MAX_ROUTE_STEPS &&
        lastCmd.action != MotionAction::STOP) {

        route[stepCount++] = {
            lastCmd,
            millis() - lastTimestamp,
            0.0f
        };
    }
}

// ==========================
// LOG COMMAND (IMU-AWARE CORE)
// ==========================
void RouteLogger::logCommand(const MotionCommand& cmd, float yaw) {

    if (!recording) return;

    uint32_t now = millis();

    // ignore duplicates (same command state)
    if (cmd.action == lastCmd.action &&
        cmd.speed == lastCmd.speed) {
        return;
    }

    // finalize previous step
    if (stepCount < MAX_ROUTE_STEPS &&
        lastCmd.action != MotionAction::STOP) {

        route[stepCount++] = {
            lastCmd,
            now - lastTimestamp,
            yaw   // 🔥 IMU SNAPSHOT STORED HERE
        };
    }

    lastCmd = cmd;
    lastTimestamp = now;
}

// ==========================
// GET STEP
// ==========================
RouteStep RouteLogger::getStep(int index) {

    if (index < 0 || index >= stepCount) {
        return { {MotionAction::STOP, 0}, 0, 0.0f };
    }

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

    lastCmd = { MotionAction::STOP, 0 };

    lastTimestamp = 0;
}

// ==========================
// STATUS
// ==========================
bool RouteLogger::isRecording() {
    return recording;
}
