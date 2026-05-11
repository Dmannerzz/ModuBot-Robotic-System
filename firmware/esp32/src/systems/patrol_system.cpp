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

    Serial.println("IMU-Assisted Patrol Started");

    executeStep(logger->getStep(currentStep));
}

// ==========================
// STOP PATROL
// ==========================
void PatrolSystem::stop() {

    running = false;

    motion->execute({ MotionAction::STOP, 0 });

    Serial.println("Patrol Replay Stopped");
}

// ==========================
// UPDATE LOOP
// ==========================
void PatrolSystem::update() {

    if (!running) return;

    RouteStep step = logger->getStep(currentStep);

    // ==========================
    // STILL EXECUTING CURRENT STEP
    // ==========================
    if (millis() - stepStartTime < step.duration) {

        // 🔥 CONTINUOUS IMU CORRECTION (KEY UPGRADE)
        motion->setYaw(step.yaw);
        motion->execute(step.cmd);

        return;
    }

    // ==========================
    // NEXT STEP
    // ==========================
    currentStep++;

    if (currentStep >= logger->getStepCount()) {

        stop();
        Serial.println("Patrol Complete");
        return;
    }

    stepStartTime = millis();

    executeStep(logger->getStep(currentStep));
}

// ==========================
// EXECUTE STEP (IMU-AWARE)
// ==========================
void PatrolSystem::executeStep(const RouteStep& step) {

    // 🔥 LOCK TARGET HEADING FROM RECORDED PATH
    motion->setYaw(step.yaw);

    // 🔥 EXECUTE USING UNIFIED COMMAND SYSTEM
    motion->execute(step.cmd);
}

// ==========================
// STATUS
// ==========================
bool PatrolSystem::isRunning() {
    return running;
}
