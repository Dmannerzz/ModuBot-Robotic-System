#include "motor.h"

// ========== Motor Pins (REAL DEFINITIONS) ==========
int MOTOR_IN1 = 5;
int MOTOR_IN2 = 6;
int MOTOR_IN3 = 7;
int MOTOR_IN4 = 8;
int ENA = 9;
int ENB = 10;
extern int currentSpeed;
int currentSpeed = 150;

void setup() {
    Serial.begin(9600);
    Serial.println("ModuBot Initializing...");

    moveForward();
    delay(1000);
    stopMotors();
}

void loop() {
    // Main control logic will go here
}
