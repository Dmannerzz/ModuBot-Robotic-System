#pragma once
#include <Arduino.h>

// ========== DEBUGGING ==========
#define DEBUG_MODE 1

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ========== MODE SYSTEM ==========
enum Mode { MANUAL, O_AVOIDANCE, PATROL, IDLE };
extern Mode currentMode;

// ========== MOTOR STATE ==========
extern int currentSpeed;

// ========== ROUTE LOGGING STATE ==========
extern bool isLoggingManualRoute;
extern int lastDirection;
extern unsigned long currentMoveStart;

// ========== DIRECTION CONSTANTS ==========
#define DIR_NONE 0
#define DIR_FORWARD 1
#define DIR_BACKWARD 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

// ========== ROUTE STRUCT ==========
struct RouteStep {
  uint8_t direction;
  unsigned long duration;
};

// ========== ROUTES ==========
extern const int MAX_ROUTE_STEPS;

extern RouteStep manualRoute[];
extern RouteStep obstacleRoute[];

extern uint8_t manualRouteIndex;
extern uint8_t obstacleRouteIndex;

// ========== PATROL STATE ==========
extern RouteStep* currentRoute;
extern uint8_t currentRouteLength;
extern uint8_t currentPatrolStep;
extern bool isPatrolRunning;
extern bool useManualRoute;
extern unsigned long patrolStepStartTime;

// ========== FUNCTIONS ==========
void logRouteStep(uint8_t dir, unsigned long duration, bool isManual);

void reportMotionEvent(uint8_t dir, unsigned long duration);
