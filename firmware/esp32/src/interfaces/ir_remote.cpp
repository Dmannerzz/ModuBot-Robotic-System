#include "ir_remote.h"
#include "../core/event_system.h"
#include <IRremote.h>

static EventQueue* queueRef;

void IRRemote::init(EventQueue* queue) {
    queueRef = queue;
    IrReceiver.begin(IR_PIN, false);
}

void IRRemote::update() {

    if (!IrReceiver.decode()) return;

    uint8_t cmd = IrReceiver.decodedIRData.command;

    switch (cmd) {
        
        // Up Arrow
        case 0x18:
            Serial.println("Move Forward");
            queueRef->push(EventType::MOVE_FORWARD, 200);
            break;
        
        // Down Arrow
        case 0x52:
            Serial.println("Move Backward");
            queueRef->push(EventType::MOVE_BACKWARD, 200);
            break;

        // Left Arrow
        case 0x08:
            Serial.println("Turn Left");
            queueRef->push(EventType::TURN_LEFT, 180);
            break;
        
        // Right Arrow
        case 0x5A:
            Serial.println("Turn Right");
            queueRef->push(EventType::TURN_RIGHT, 180);
            break;

        // OK Button
        case 0x1C:
            Serial.println("Stop");
            queueRef->push(EventType::STOP);
            break;

        // Button 1
        case 0x45:
            Serial.println("Switch to Manual Mode");
            queueRef->push(EventType::MODE_MANUAL);
            break;

        // Button 2
        case 0x46:
            Serial.println("Switch to Obstacle Avoidance Mode");
            queueRef->push(EventType::MODE_OBSTACLE);
            break;

        // Button 4
        case 0x44:
            Serial.println("Switch to Patrol Mode");
            queueRef->push(EventType::MODE_PATROL);
            break;

        // Button 5
        case 0x40:
            Serial.println("Switch to Idle Mode");
            queueRef->push(EventType::MODE_IDLE);
            break;
    }

    IrReceiver.resume();
}
