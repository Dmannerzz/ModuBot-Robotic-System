#include "motor.h"
#include "globals.h"
#include <Arduino.h>

// pins
extern int MOTOR_IN1;
extern int MOTOR_IN2;
extern int MOTOR_IN3;
extern int MOTOR_IN4;
extern int ENA;
extern int ENB;

// speed
extern int currentSpeed;

void setMotor(int in1, int in2, int in3, int in4, int speed) {
  digitalWrite(MOTOR_IN1, in1);
  digitalWrite(MOTOR_IN2, in2);
  digitalWrite(MOTOR_IN3, in3);
  digitalWrite(MOTOR_IN4, in4);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// ========== MOTION API ==========
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

void stopMotors() {
  setMotor(LOW, LOW, LOW, LOW, 0);
}

void executeMotion(uint8_t dir) {

    if (obstacleActive) {
        stopMotors();
        return;
    }

    switch (dir) {
        case DIR_FORWARD: moveForward(); break;
        case DIR_BACKWARD: moveBackward(); break;
        case DIR_LEFT: turnLeft(); break;
        case DIR_RIGHT: turnRight(); break;
        default: stopMotors(); break;
    }
}
