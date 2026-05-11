#include <Arduino.h>

#include "core/event_system.h"
#include "core/state_machine.h"
#include "core/globals.h"

#include "drivers/motors.h"

#include "systems/obstacle_system.h"
#include "systems/patrol_system.h"

#include "interfaces/ir_remote.h"

// ==========================
// GLOBAL SYSTEM OBJECTS
// ==========================
EventQueue eventQueue;

StateMachine stateMachine;

ObstacleSystem obstacleSystem;

IRRemote irRemote;

// ==========================
// SETUP
// ==========================
void setup() {

    Serial.begin(115200);

    // ==========================
    // DRIVER INIT
    // ==========================
    Motors::begin();

    // ==========================
    // SYSTEM INIT
    // ==========================
    stateMachine.init(&eventQueue);

    obstacleSystem.begin(&eventQueue);

    irRemote.init(&eventQueue);

    Serial.println("ModuBot ESP32 System Started");
}

// ==========================
// MAIN LOOP
// ==========================
void loop() {

    unsigned long now = millis();

    // ==========================
    // GLOBAL SYSTEM TICK (50Hz)
    // ==========================
    if (now - lastTick >= 20) {

        lastTick = now;

        eventQueue.push(EventType::TIMER_TICK);
    }

    // ==========================
    // INPUT LAYER
    // ==========================
    irRemote.update();

    // ==========================
    // SENSOR SYSTEMS
    // ==========================
    obstacleSystem.update();

    // ==========================
    // CORE ROBOT BRAIN
    // ==========================
    stateMachine.update();
}
