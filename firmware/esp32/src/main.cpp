#include <Arduino.h>
#include "event_system.h"
#include "state_machine.h"
#include "motors.h"

// ==========================
// GLOBAL SYSTEM OBJECTS
// ==========================
EventQueue eventQueue;
StateMachine stateMachine;

// ==========================
// SETUP
// ==========================
void setup() {
    Serial.begin(115200);

    // Init subsystems
    stateMachine.init(&eventQueue);

    Serial.println("ModuBot ESP32 System Started");
}

// ==========================
// LOOP
// ==========================
void loop() {

    // 1. SYSTEM TICK
    stateMachine.update();

    // 2. SIMULATION / TEST INPUTS (REMOVE LATER)
    static unsigned long lastTest = 0;

    if (millis() - lastTest > 3000) {
        lastTest = millis();

        // Example test event
        eventQueue.push(EventType::MOVE_FORWARD, 200);
    }

    delay(10);
}
