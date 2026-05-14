#pragma once

#include <Arduino.h>
#include "route_logger.h"
#include "../motion/motion_engine.h"

#define MAX_ROUTES 2  // Support 2 routes

class PatrolSystem {
public:

    void begin(RouteLogger* loggerRef,
               MotionEngine* motionRef);

    void start();
    void stop();

    void pause();
    void resume();

    void update();

    bool isRunning();
    bool isPaused();

    // Route selection
    void selectRoute(int routeIndex);
    int getSelectedRoute();
    void startWithRoute(int routeIndex);

private:

    RouteLogger* logger = nullptr;
    MotionEngine* motion = nullptr;

    bool running = false;
    bool paused = false;

    int currentStep = 0;
    int selectedRoute = 0;  // Currently selected route (0 or 1)

    unsigned long stepStartTime = 0;
    unsigned long pausedElapsed = 0;

    void executeStep(const RouteStep& step);
};
