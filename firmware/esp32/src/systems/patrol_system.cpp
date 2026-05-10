#include "patrol_system.h"

// ==========================
// INIT
// ==========================
void PatrolSystem::begin(RouteLogger* loggerRef,
                         MotionEngine* motionRef) {

    logger = loggerRef;
    motion = motionRef;
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

    currentStep = 0;

    stepStartTime = millis();

    Serial.println("Patrol Replay Started");

    executeStep(logger->getStep(currentStep));
}

// ==========================
// STOP PATROL
// ==========================
void PatrolSystem::stop() {

    running = false;

    motion->stop();

    Serial.println("Patrol Replay Stopped");
}

// ==========================
// UPDATE LOOP
// ==========================
void PatrolSystem::update() {

    if (!running) return;

    // ==========================
    // SAFETY OVERRIDE
    // ==========================
    if (motion->isSafetyOverrideActive()) {

        Serial.println("Patrol Interrupted: Obstacle");

        stop();

        return;
    }

    RouteStep step = logger->getStep(currentStep);

    // ==========================
    // STEP STILL ACTIVE
    // ==========================
    if (millis() - stepStartTime < step.duration) {
        return;
    }

    // ==========================
    // NEXT STEP
    // ==========================
    currentStep++;

    // ==========================
    // PATROL COMPLETE
    // ==========================
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

    switch (step.action) {

        case EventType::MOVE_FORWARD:
            motion->forward(patrolSpeed);
            break;

        case EventType::MOVE_BACKWARD:
            motion->backward(patrolSpeed);
            break;

        case EventType::TURN_LEFT:
            motion->left(patrolSpeed);
            break;

        case EventType::TURN_RIGHT:
            motion->right(patrolSpeed);
            break;

        case EventType::STOP:
            motion->stop();
            break;

        default:
            motion->stop();
            break;
    }
}

// ==========================
// STATUS
// ==========================
bool PatrolSystem::isRunning() {

    return running;
}
