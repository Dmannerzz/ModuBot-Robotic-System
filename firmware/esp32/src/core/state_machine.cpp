#include "state_machine.h"

#include "motion/motion_engine.h"
#include "systems/route_logger.h"
#include "systems/patrol_system.h"
#include "core/control_policy.h"
#include "motion/motion_command.h"
#include "drivers/imu.h"

// ==========================
// INTERNAL SYSTEMS
// ==========================
static MotionEngine motion;
static RouteLogger logger;
static PatrolSystem patrol;
static ControlPolicy policy;
static IMU imu;   

// ==========================
// INIT
// ==========================
void StateMachine::init(EventQueue* queue) {

    eventQueue = queue;
    currentState = RobotState::IDLE;

    // ==========================
    // CORE MODULE INIT
    // ==========================
    motion.begin();
    logger.begin();

    patrol.begin(&logger, &motion);

    policy.setAuthority(ControlAuthority::NONE);

    // ==========================
    // IMU INIT + WIRING
    // ==========================
    imu.begin();
    motion.attachIMU(&imu);

    motion.setSafetyOverride(false);

    // SAFE START STATE
    motion.execute({MotionAction::STOP, 0});

    Serial.println("System Wiring Complete");
}

// ==========================
// UPDATE LOOP
// ==========================
void StateMachine::update() {

    Event event;

    while (eventQueue->pop(event)) {
        handleEvent(event);
    }

    patrol.update();
}

// ==========================
// EVENT ROUTER
// ==========================
void StateMachine::handleEvent(const Event& event) {

    // ==========================
    // SAFETY OVERRIDE (HIGHEST PRIORITY)
    // ==========================
    if (event.type == EventType::OBSTACLE_DETECTED) {

        policy.emergencyStop();
        patrol.stop();

        motion.setSafetyOverride(true);

        transitionTo(RobotState::OBSTACLE_AVOIDANCE);

        motion.execute({MotionAction::STOP, 0});

        return;
    }

    if (event.type == EventType::OBSTACLE_CLEARED &&
        currentState == RobotState::OBSTACLE_AVOIDANCE) {

        policy.resetEmergency();
        motion.setSafetyOverride(false);

        transitionTo(RobotState::MANUAL);

        motion.execute({MotionAction::STOP, 0});

        return;
    }

    // ==========================
    // MODE SWITCHING
    // ==========================
    switch (event.type) {

        case EventType::MODE_MANUAL:
            patrol.stop();
            transitionTo(RobotState::MANUAL);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::MODE_OBSTACLE:
            patrol.stop();
            transitionTo(RobotState::OBSTACLE_AVOIDANCE);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::MODE_PATROL:
            transitionTo(RobotState::PATROL);
            patrol.start();
            return;

        case EventType::MODE_IDLE:
            patrol.stop();
            transitionTo(RobotState::IDLE);
            motion.execute({MotionAction::STOP, 0});
            return;

        case EventType::LOG_ROUTE:

            if (!logger.isRecording()) {
                logger.startRecording();
                Serial.println("Route Recording Started");
            } else {
                logger.stopRecording();
                Serial.println("Route Recording Stopped");
            }
            return;

        case EventType::RESET_LOGS:
            logger.clear();
            Serial.println("Route Cleared");
            return;

        default:
            break;
    }

    // ==========================
    // STATE DISPATCH
    // ==========================
    switch (currentState) {

        case RobotState::MANUAL:
            handleManual(event);
            break;

        case RobotState::OBSTACLE_AVOIDANCE:
            handleObstacle(event);
            break;

        case RobotState::PATROL:
            handlePatrol(event);
            break;

        case RobotState::IDLE:
            handleIdle(event);
            break;
    }
}
