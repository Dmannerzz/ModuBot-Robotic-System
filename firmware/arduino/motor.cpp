#include "motor.h"
#include <Arduino.h>

extern int currentSpeed;

// pins come from main (for now)
extern int MOTOR_IN1;
extern int MOTOR_IN2;
extern int MOTOR_IN3;
extern int MOTOR_IN4;
extern int ENA;
extern int ENB;

void setMotor(int in1, int in2, int in3, int in4, int speed) {
  digitalWrite(MOTOR_IN1, in1);
  digitalWrite(MOTOR_IN2, in2);
  digitalWrite(MOTOR_IN3, in3);
  digitalWrite(MOTOR_IN4, in4);

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void stopMotors() {
  setMotor(LOW, LOW, LOW, LOW, 0);
}
