#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// ========== BASIC MOVEMENT ==========
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void rotateLeft();
void rotateRight();
void stopMotors();

// ========== MOTION ENGINE (NEW) ==========
void executeMotion(uint8_t dir);

#endif
