#include "motor.h"
#include "globals.h"
#include <Arduino.h>

// ========== PIN REFERENCES (from main) ==========
extern int MOTOR_IN1;
extern int MOTOR_IN2;
extern int MOTOR_IN3;
extern int MOTOR_IN4;
extern int ENA;
extern int ENB;

// ========== CORE MOTOR CONTROL ==========
void setMotor(int in1, int in2, int in3, int in4, int speed) {
  digitalWrite(MOTOR_IN1, in1);
  digitalWrite(MOTOR_IN2, in2);
  digitalWrite(MOTOR_IN3, in3);
  digitalWrite(MOTOR_IN4, in4);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// ========== MOVEMENT ==========
void moveForward() {
  currentMoveStart = millis();
  lastDirection = DIR_FORWARD;
  setMotor(LOW, HIGH, LOW, HIGH, currentSpeed);
}

void moveBackward() {
  currentMoveStart = millis();
  lastDirection = DIR_BACKWARD;
  setMotor(HIGH, LOW, HIGH, LOW, currentSpeed);
}

void turnLeft() {
  currentMoveStart = millis();
  lastDirection = DIR_LEFT;
  setMotor(LOW, HIGH, HIGH, LOW, currentSpeed);
}

void turnRight() {
  currentMoveStart = millis();
  lastDirection = DIR_RIGHT;
  setMotor(HIGH, LOW, LOW, HIGH, currentSpeed);
}

// Optional (keep if needed)
void rotateLeft() {
  setMotor(LOW, HIGH, HIGH, LOW, currentSpeed);
}

void rotateRight() {
  setMotor(HIGH, LOW, LOW, HIGH, currentSpeed);
}

// ========== STOP + LOGGING ==========
void stopMotors() {

  setMotor(LOW, LOW, LOW, LOW, 0);

  unsigned long duration = millis() - currentMoveStart;

  // ONLY report motion event, no decision making
  if (lastDirection != DIR_NONE && duration > 50) {
      reportMotionEvent(lastDirection, duration);
  }

  lastDirection = DIR_NONE;
}

void executeMotion(uint8_t dir) {
    switch (dir) {
        case DIR_FORWARD: moveForward(); break;
        case DIR_BACKWARD: moveBackward(); break;
        case DIR_LEFT: turnLeft(); break;
        case DIR_RIGHT: turnRight(); break;
        default: stopMotors(); break;
    }
}
