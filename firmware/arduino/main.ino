#include "motor.h"
#include <IRremote.h>

// ========== IR ==========
#define IR_PIN 11

// ========== Motor Pins ==========
int MOTOR_IN1 = 5;
int MOTOR_IN2 = 6;
int MOTOR_IN3 = 7;
int MOTOR_IN4 = 8;
int ENA = 9;
int ENB = 10;

// ========== Speed ==========
int currentSpeed = 150;

// ========== STATE ==========
bool isIdle = false;

// ========== MODE SYSTEM ==========
enum Mode { MANUAL, O_AVOIDANCE, PATROL, IDLE };
Mode currentMode = IDLE;

// ========== FUNCTION PROTOTYPES ==========
void runCurrentMode();
void handleManual();
void handleObstacle();
void handlePatrol();
void handleIdle();
void setMode(Mode newMode);
void handleIR(uint8_t command);

// ========== PATROL STATE VARIABLES ==========
struct RouteStep {
  uint8_t direction;
  unsigned long duration;
};

const int MAX_ROUTE_STEPS = 30;

RouteStep manualRoute[MAX_ROUTE_STEPS];
RouteStep obstacleRoute[MAX_ROUTE_STEPS];

uint8_t manualRouteIndex = 0;
uint8_t obstacleRouteIndex = 0;

RouteStep* currentRoute = nullptr;
uint8_t currentRouteLength = 0;
uint8_t currentPatrolStep = 0;

bool isPatrolRunning = false;
bool useManualRoute = true;

unsigned long patrolStepStartTime = 0;

// ========== SETUP ==========
void setup() {
    Serial.begin(9600);
    Serial.println("ModuBot Ready");

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
    switch (command) {

        case 0x45:
            setMode(MANUAL);
            break;

        case 0x46:
            setMode(O_AVOIDANCE);
            break;

        case 0x44:
            setMode(PATROL);
            break;

        case 0x40:
            setMode(IDLE);
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
