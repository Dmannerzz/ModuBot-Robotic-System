#pragma once
#include <Arduino.h>
#include "event_system.h"
#include "motion_command.h"

#define MAX_ROUTE_STEPS 50

struct RouteStep {
    MotionCommand action;
    uint32_t duration;
};

class RouteLogger {
public:
    void begin();

    void startRecording();
    void stopRecording();

    void logEvent(EventType type);

    RouteStep getStep(int index);
    int getStepCount();

    void clear();

    bool isRecording();

private:
    RouteStep route[MAX_ROUTE_STEPS];

    bool recording = false;

    int stepCount = 0;

    EventType lastAction = EventType::NONE;

    uint32_t lastTimestamp = 0;

    MotionCommand toMotion(EventType type);
};
