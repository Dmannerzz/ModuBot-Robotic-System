#include <Arduino.h>

#include "core/event_system.h"
#include "core/state_machine.h"

#include "drivers/motors.h"

#include "systems/obstacle_system.h"
#include "systems/patrol_system.h"

#include "interfaces/ir_remote.h"

// ==========================
// GLOBAL SYSTEMS
// ==========================
EventQueue eventQueue;

StateMachine stateMachine;
ObstacleSystem obstacleSystem;
PatrolSystem patrolSystem;
IRRemote irRemote;

// ==========================
// TIMER
// ==========================
static unsigned long lastTick = 0;

// ==========================
// SETUP
// ==========================
void setup() {

    Serial.begin(115200);

    Motors::begin();

    stateMachine.init(&eventQueue);

    obstacleSystem.begin(&eventQueue);

    irRemote.init(&eventQueue);

    Serial.println("ModuBot System Ready");
}

// ==========================
// LOOP
// ==========================
void loop() {

    unsigned long now = millis();

    // 50Hz system tick
    if (now - lastTick >= 20) {

        lastTick = now;

        eventQueue.push(EventType::TIMER_TICK);
    }

    irRemote.update();

    obstacleSystem.update();

    patrolSystem.update();

    stateMachine.update();
}
