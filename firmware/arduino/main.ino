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

// ========== SETUP ==========
void setup() {
    Serial.begin(9600);
    DEBUG_PRINTLN("ModuBot Ready");

    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}

// ========== LOOP ==========
void loop() {
    if (IrReceiver.decode()) {
        uint8_t command = IrReceiver.decodedIRData.command;
        handleIR(command);
        IrReceiver.resume();
    }

    runCurrentMode();
}

// ========== IR HANDLER ==========
void handleIR(uint8_t command) {

    // ================= MODE SWITCHING =================
    switch (command) {

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

        // ================= ROUTE LOGGING START =================
        case 0x07: // Button 7 → START LOGGING
            if (currentMode == MANUAL) {
                isLoggingManualRoute = true;
                manualRouteIndex = 0;
                DEBUG_PRINTLN("Manual route logging started");
            } else {
                DEBUG_PRINTLN("Not in MANUAL mode");
            }
            break;

        // ================= MANUAL MOVEMENT =================
        case 0x18: // UP
            if (currentMode == MANUAL) moveForward();
            break;

        case 0x52: // DOWN
            if (currentMode == MANUAL) moveBackward();
            break;

        case 0x08: // LEFT
            if (currentMode == MANUAL) turnLeft();
            break;

        case 0x5A: // RIGHT
            if (currentMode == MANUAL) turnRight();
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

    isIdle = false;

    if (!isPatrolRunning) {

        currentRoute = useManualRoute ? manualRoute : obstacleRoute;
        currentRouteLength = useManualRoute ? manualRouteIndex : obstacleRouteIndex;

        // Safety check
        if (currentRouteLength == 0) {
            return;
        }

        currentPatrolStep = 0;
        patrolStepStartTime = millis();
        isPatrolRunning = true;
    }

    executePatrolStep();
}

void executePatrolStep() {

    if (!isPatrolRunning || currentRoute == nullptr) return;

    if (currentPatrolStep >= currentRouteLength) {
        stopMotors();
        currentMode = IDLE;
        isPatrolRunning = false;
        return;
    }

    RouteStep step = currentRoute[currentPatrolStep];

    switch (step.direction) {
        case 1: moveForward(); break;
        case 2: moveBackward(); break;
        case 3: turnLeft(); break;
        case 4: turnRight(); break;
        default: stopMotors(); break;
    }

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
