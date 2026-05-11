#pragma once
#include <Arduino.h>
#include "motion_command.h"

#define MAX_ROUTE_STEPS 50

struct RouteStep {
    MotionCommand cmd;
    uint32_t duration;
};

class RouteLogger {
public:
    void begin();

    void startRecording();
    void stopRecording();

    void logCommand(const MotionCommand& cmd);

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
