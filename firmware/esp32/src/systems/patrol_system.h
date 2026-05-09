#pragma once

#include <Arduino.h>
#include "route_logger.h"
#include "motion_engine.h"

class PatrolSystem {
public:

    void begin(RouteLogger* loggerRef,
               MotionEngine* motionRef);

    void start();

    void stop();

    void update();

    bool isRunning();

private:

    RouteLogger* logger = nullptr;

    MotionEngine* motion = nullptr;

    bool running = false;

    int currentStep = 0;

    unsigned long stepStartTime = 0;

    void executeStep(const RouteStep& step);
};
