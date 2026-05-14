#include "patrol_system.h"

// ==========================
// INIT
// ==========================
void PatrolSystem::begin(RouteLogger* loggerRef,
                         MotionEngine* motionRef) {

    logger = loggerRef;
    motion = motionRef;
    selectedRoute = 0;
}

// ==========================
// SELECT ROUTE
// ==========================
void PatrolSystem::selectRoute(int routeIndex) {

    if (routeIndex < 0 || routeIndex >= MAX_ROUTES) {
        Serial.println("Invalid route index");
        return;
    }

    selectedRoute = routeIndex;

    Serial.print("Selected Route: ");
    Serial.println(selectedRoute);
}

// ==========================
// GET SELECTED ROUTE
// ==========================
int PatrolSystem::getSelectedRoute() {
    return selectedRoute;
}

// ==========================
// START WITH SELECTED ROUTE
// ==========================
void PatrolSystem::startWithRoute(int routeIndex) {

    selectRoute(routeIndex);
    start();
}

// ==========================
// START PATROL
// ==========================
void PatrolSystem::start() {

    if (logger->getStepCount() == 0) {

        Serial.println("No Route Recorded");
        return;
    }

    running = true;
    paused = false;

    currentStep = 0;

    pausedElapsed = 0;

    stepStartTime = millis();

    Serial.print("IMU-Assisted Patrol Started (Route ");
    Serial.print(selectedRoute);
    Serial.println(")");

    executeStep(logger->getStep(currentStep));
}

// ==========================
// STOP PATROL
// ==========================
void PatrolSystem::stop() {

    running = false;
    paused = false;

    motion->execute({
        MotionAction::STOP,
        0
    });

    Serial.println("Patrol Replay Stopped");
}

// ==========================
// PAUSE PATROL
// ==========================
void PatrolSystem::pause() {

    if (!running || paused) {
        return;
    }

    paused = true;

    // preserve elapsed time
    pausedElapsed += millis() - stepStartTime;

    motion->execute({
        MotionAction::STOP,
        0
    });

    Serial.println("Patrol Paused");
}

// ==========================
// RESUME PATROL
// ==========================
void PatrolSystem::resume() {

    if (!running || !paused) {
        return;
    }

    paused = false;

    stepStartTime = millis();

    executeStep(logger->getStep(currentStep));

    Serial.println("Patrol Resumed");
}

// ==========================
// UPDATE LOOP
// ==========================
void PatrolSystem::update() {

    if (!running || paused) {
        return;
    }

    RouteStep step = logger->getStep(currentStep);

    unsigned long elapsed =
        pausedElapsed +
        (millis() - stepStartTime);

    // ==========================
    // STILL EXECUTING STEP
    // ==========================
    if (elapsed < step.duration) {

        motion->setYaw(step.yaw);

        motion->execute(step.cmd);

        return;
    }

    // ==========================
    // NEXT STEP
    // ==========================
    currentStep++;

    pausedElapsed = 0;

    if (currentStep >= logger->getStepCount()) {

        stop();

        Serial.println("Patrol Complete");

        return;
    }

    stepStartTime = millis();

    executeStep(logger->getStep(currentStep));
}

// ==========================
// EXECUTE STEP
// ==========================
void PatrolSystem::executeStep(const RouteStep& step) {

    motion->setYaw(step.yaw);

    motion->execute(step.cmd);
}

// ==========================
// STATUS
// ==========================
bool PatrolSystem::isRunning() {
    return running;
}

// ==========================
// PAUSE STATUS
// ==========================
bool PatrolSystem::isPaused() {
    return paused;
}
