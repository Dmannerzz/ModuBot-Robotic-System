#pragma once
#include <Arduino.h>

#define DEBUG_MODE 1

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ========== SENSOR PINS ==========
extern const int TRIG_PIN;
extern const int ECHO_PIN;

// ========== MODE ==========
enum Mode { MANUAL, O_AVOIDANCE, PATROL, IDLE };
extern Mode currentMode;

// ========== MOTOR ==========
extern int currentSpeed;

// ========== DIRECTIONS ==========
#define DIR_NONE 0
#define DIR_FORWARD 1
#define DIR_BACKWARD 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

// ========== SENSOR STATE ==========
extern bool obstacleActive;

// ========== ROUTE LOGGING ==========
extern bool isLoggingManualRoute;
extern int lastDirection;
extern unsigned long currentMoveStart;

// ========== ROUTES ==========
struct RouteStep {
  uint8_t direction;
  unsigned long duration;
};

extern const int MAX_ROUTE_STEPS;

extern RouteStep manualRoute[];
extern RouteStep obstacleRoute[];

extern uint8_t manualRouteIndex;
extern uint8_t obstacleRouteIndex;

// ========== PATROL ==========
extern RouteStep* currentRoute;
extern uint8_t currentRouteLength;
extern uint8_t currentPatrolStep;

extern bool isPatrolRunning;
extern bool useManualRoute;
extern unsigned long patrolStepStartTime;

void updateObstacleSensor();
long readDistanceCM();
void executeMotion(uint8_t dir);
