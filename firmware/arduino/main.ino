#include "motor.h"

void setup() {
    Serial.begin(9600);
    Serial.println("ModuBot Initializing...");

    moveForward();
    delay(1000);
    stopMoving();
}

void loop() {
    // Main control logic will go here
}
