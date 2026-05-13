#pragma once

#include <Arduino.h>
#include "../core/motion_command.h"

#define MAX_ROUTE_STEPS 50

// ==========================
// ROUTE STEP (IMU-AWARE)
// ==========================
struct RouteStep {
    MotionCommand cmd;
    uint32_t duration;

    // 🔥 NEW: heading reference for IMU replay correction
    float yaw;
};

// ==========================
// ROUTE LOGGER
// ==========================
class RouteLogger {
public:
    void begin();

    void startRecording();
    void stopRecording();

    // 🔥 UPDATED: now includes yaw capture
    void logCommand(const MotionCommand& cmd, float yaw);

    RouteStep getStep(int index);
    int getStepCount();

    void clear();
    bool isRecording();

private:
    RouteStep route[MAX_ROUTE_STEPS];

    bool recording = false;
    int stepCount = 0;

    MotionCommand lastCmd;
    uint32_t lastTimestamp = 0;
};
