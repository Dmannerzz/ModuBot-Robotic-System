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
            queueRef->push(EventType::MOVE_FORWARD, 255);  // Max speed
            break;
        
        // Down Arrow
        case 0x52:
            Serial.println("Move Backward");
            queueRef->push(EventType::MOVE_BACKWARD, 255);  // Max speed
            break;

        // Left Arrow
        case 0x08:
            Serial.println("Turn Left");
            queueRef->push(EventType::TURN_LEFT, 255);  // Max speed
            break;
        
        // Right Arrow
        case 0x5A:
            Serial.println("Turn Right");
            queueRef->push(EventType::TURN_RIGHT, 255);  // Max speed
            break;

        // OK Button
        case 0x1C:
            Serial.println("Stop");
            queueRef->push(EventType::STOP);
            break;

        // Button 1 - Manual Mode
        case 0x45:
            Serial.println("Switch to Manual Mode");
            queueRef->push(EventType::MODE_MANUAL);
            break;

        // Button 2 - Obstacle Avoidance Mode
        case 0x46:
            Serial.println("Switch to Obstacle Avoidance Mode");
            queueRef->push(EventType::MODE_OBSTACLE);
            break;

        // Button 3 - Patrol Mode
        case 0x47:
            Serial.println("Switch to Patrol Mode");
            queueRef->push(EventType::MODE_PATROL);
            break;

        // Button 4 - Select Route 0
        case 0x44:
            Serial.println("Select Route 0");
            queueRef->push(EventType::SELECT_ROUTE_0);
            break;

        // Button 5 - Select Route 1
        case 0x40:
            Serial.println("Select Route 1");
            queueRef->push(EventType::SELECT_ROUTE_1);
            break;

        // Button 6 - Idle Mode
        case 0x43:
            Serial.println("Switch to Idle Mode");
            queueRef->push(EventType::MODE_IDLE);
            break;

        // Button 7 - Toggle Route Recording (Manual Mode)
        case 0x07:
            Serial.println("Toggle Route Recording");
            queueRef->push(EventType::LOG_ROUTE);
            break;
    }

    IrReceiver.resume();
}
