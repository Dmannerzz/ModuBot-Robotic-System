#include "motor.h"
#include <IRremote.h>
#include "globals.h"

// ========== IR ==========
#define IR_PIN 11

// ========== MOTOR PINS ==========
int MOTOR_IN1 = 5;
int MOTOR_IN2 = 6;
int MOTOR_IN3 = 7;
int MOTOR_IN4 = 8;
int ENA = 9;
int ENB = 10;

// ========== ULTRASONIC SENSOR PINS ==========
const int TRIG_PIN = 3;
const int ECHO_PIN = 4;

// ========== SPEED ==========
int currentSpeed = 150;

// ========== STATE ==========
bool isIdle = false;

// ========== MODE SYSTEM ==========
Mode currentMode = IDLE;

// ========== ROUTE LOGGING ==========
bool isLoggingManualRoute = false;
int lastDirection = DIR_NONE;
unsigned long currentMoveStart = 0;

// ========== ROUTES ==========
const int MAX_ROUTE_STEPS = 30;

RouteStep manualRoute[MAX_ROUTE_STEPS];
RouteStep obstacleRoute[MAX_ROUTE_STEPS];

uint8_t manualRouteIndex = 0;
uint8_t obstacleRouteIndex = 0;

// ========== PATROL ==========
RouteStep* currentRoute = nullptr;
uint8_t currentRouteLength = 0;
uint8_t currentPatrolStep = 0;

bool isPatrolRunning = false;
bool useManualRoute = true;

unsigned long patrolStepStartTime = 0;

// ========== PATROL TIMING CONTROL ==========
unsigned long lastPatrolUpdate = 0;
const unsigned long PATROL_TICK = 20;

// ========== OBSTACLE ==========
bool obstacleDetected = false;
bool obstacleActive = false;
unsigned long obstacleStartTime = 0;

bool patrolPaused = false;
unsigned long patrolPauseTime = 0;

// ========== SETUP ==========
void setup() {
    Serial.begin(9600);
    DEBUG_PRINTLN("ModuBot Ready");

    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}

// ========== LOOP ==========
void loop() {

    updateObstacleSensor();   // perception layer

    if (IrReceiver.decode()) {
        handleIR(IrReceiver.decodedIRData.command);
        IrReceiver.resume();
    }

    runCurrentMode();         // behavior layer
}

// ========== IR HANDLER ==========
void handleIR(uint8_t command) {

    switch (command) {

        // ================= MODE SWITCHING =================
        case 0x45: // MANUAL
            isLoggingManualRoute = false;
            setMode(MANUAL);
            break;

        case 0x46: // OBSTACLE
            isLoggingManualRoute = false;
            setMode(O_AVOIDANCE);
            break;

        case 0x44: // PATROL
            isLoggingManualRoute = false;
            setMode(PATROL);
            break;

        case 0x40: // IDLE
            isLoggingManualRoute = false;
            setMode(IDLE);
            break;

        // ================= ROUTE LOGGING =================
        case 0x07:
            if (currentMode == MANUAL) {
                isLoggingManualRoute = true;
                manualRouteIndex = 0;
                DEBUG_PRINTLN("Manual route logging started");
            } else {
                DEBUG_PRINTLN("Not in MANUAL mode");
            }
            break;

        // ================= MANUAL MOVEMENT (FIXED) =================
        case 0x18: // UP
            if (currentMode == MANUAL) executeMotion(DIR_FORWARD);
            break;

        case 0x52: // DOWN
            if (currentMode == MANUAL) executeMotion(DIR_BACKWARD);
            break;

        case 0x08: // LEFT
            if (currentMode == MANUAL) executeMotion(DIR_LEFT);
            break;

        case 0x5A: // RIGHT
            if (currentMode == MANUAL) executeMotion(DIR_RIGHT);
            break;

        case 0x1C: // STOP (OK BUTTON)
            if (currentMode == MANUAL) stopMotors();
            break;

        default:
            break;
    }
}

// ========== MODE DISPATCH ==========
void runCurrentMode() {
    switch (currentMode) {

        case MANUAL:
            handleManual();
            break;

        case O_AVOIDANCE:
            handleObstacle();
            break;

        case PATROL:
            handlePatrol();
            break;

        case IDLE:
        default:
            handleIdle();
            break;
    }
}

// ========== MODE CONTROL ==========
void setMode(Mode newMode) {
    currentMode = newMode;

    stopMotors();

    // Reset patrol when entering PATROL
    if (newMode == PATROL) {
        isPatrolRunning = false;
        currentPatrolStep = 0;
        patrolStepStartTime = millis();
    }
}

// ========== MODE HANDLERS ==========
void handleManual() {
    isIdle = false;
}

void handleObstacle() {
    isIdle = false;
}

void handlePatrol() {

    if (obstacleActive) {
        stopMotors();
        return;
    }

    if (!isPatrolRunning) {

        currentRoute = useManualRoute ? manualRoute : obstacleRoute;
        currentRouteLength = useManualRoute ? manualRouteIndex : obstacleRouteIndex;

        if (currentRouteLength == 0) return;

        currentPatrolStep = 0;
        patrolStepStartTime = millis();
        isPatrolRunning = true;
    }

    executePatrolStep();
}

void executePatrolStep() {

    if (!isPatrolRunning || currentRoute == nullptr) return;

    // 🔴 OBSTACLE SAFETY LAYER
    if (obstacleActive) {
        stopMotors();
        return;
    }

    if (currentPatrolStep >= currentRouteLength) {
        stopMotors();
        currentMode = IDLE;
        isPatrolRunning = false;
        return;
    }

    RouteStep step = currentRoute[currentPatrolStep];

    // 🧠 USE UNIFIED MOTION ENGINE (IMPORTANT CHANGE)
    executeMotion(step.direction);

    // ⏱️ TIMING CONTROL (NON-BLOCKING)
    if (millis() - patrolStepStartTime >= step.duration) {
        stopMotors();
        currentPatrolStep++;
        patrolStepStartTime = millis();
    }
}

void handleIdle() {
    stopMotors();
}

void logRouteStep(uint8_t dir, unsigned long duration, bool isManual) {

    if (isManual) {

        if (!isLoggingManualRoute) return;

        if (manualRouteIndex >= MAX_ROUTE_STEPS) return;

        manualRoute[manualRouteIndex++] = { dir, duration };

        DEBUG_PRINT("Logged: ");
        DEBUG_PRINT(dir);
        DEBUG_PRINT(" | ");
        DEBUG_PRINTLN(duration);
    }
}

void reportMotionEvent(uint8_t dir, unsigned long duration) {

    if (currentMode == MANUAL && isLoggingManualRoute) {
        logRouteStep(dir, duration, true);
    }

    // Future:
    // else if (currentMode == O_AVOIDANCE) log obstacle route
}

void triggerObstacle() {

    if (obstacleActive) return; // already handling it

    obstacleActive = true;
    obstacleStartTime = millis();

    stopMotors(); // IMMEDIATE HARD STOP

    DEBUG_PRINTLN("OBSTACLE TRIGGERED");
}

void clearObstacle() {

    if (!obstacleActive) return;

    obstacleActive = false;

    DEBUG_PRINTLN("OBSTACLE CLEARED");
}
