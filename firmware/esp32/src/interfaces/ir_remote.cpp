#include "ir_remote.h"
#include "event_system.h"

static EventQueue* queueRef;

void IRRemote::init(EventQueue* queue) {
    queueRef = queue;
    IrReceiver.begin(IR_PIN, false);
}

void IRRemote::update() {

    if (!IrReceiver.decode()) return;

    uint8_t cmd = IrReceiver.decodedIRData.command;

    switch (cmd) {

        case 0x18:
            queueRef->push(EventType::MOVE_FORWARD, 200);
            break;

        case 0x52:
            queueRef->push(EventType::MOVE_BACKWARD, 200);
            break;

        case 0x08:
            queueRef->push(EventType::TURN_LEFT, 180);
            break;

        case 0x5A:
            queueRef->push(EventType::TURN_RIGHT, 180);
            break;

        case 0x1C:
            queueRef->push(EventType::STOP);
            break;

        case 0x45:
            queueRef->push(EventType::MODE_MANUAL);
            break;

        case 0x46:
            queueRef->push(EventType::MODE_OBSTACLE);
            break;

        case 0x44:
            queueRef->push(EventType::MODE_PATROL);
            break;

        case 0x40:
            queueRef->push(EventType::MODE_IDLE);
            break;
    }

    IrReceiver.resume();
}
