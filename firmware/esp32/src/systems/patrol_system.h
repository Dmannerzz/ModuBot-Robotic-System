#pragma once

#include <Arduino.h>
#include "route_storage.h"
#include "../motion/motion_engine.h"

#define MAX_ROUTES 2

class PatrolSystem {
public:

    void begin(RouteStorage* storageRef,
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

    RouteStorage* storage = nullptr;
    MotionEngine* motion = nullptr;

    bool running = false;
    bool paused = false;

    int currentStep = 0;
    int selectedRoute = 0;

    unsigned long stepStartTime = 0;
    unsigned long pausedElapsed = 0;

    void executeStep(const RouteStep& step);
};
