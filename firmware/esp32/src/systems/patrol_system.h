#pragma once

#include <Arduino.h>
#include "route_logger.h"
#include "../motion/motion_engine.h"

class PatrolSystem {
public:

    void begin(RouteLogger* loggerRef,
               MotionEngine* motionRef);

    void start();
    void stop();

    // NEW
    void pause();
    void resume();

    void update();

    bool isRunning();
    bool isPaused();

private:

    RouteLogger* logger = nullptr;
    MotionEngine* motion = nullptr;

    bool running = false;
    bool paused = false;

    int currentStep = 0;

    unsigned long stepStartTime = 0;

    // NEW
    unsigned long pausedElapsed = 0;

    void executeStep(const RouteStep& step);
};
