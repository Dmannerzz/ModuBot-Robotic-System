#include "event_system.h"

void EventQueue::push(EventType type, int value) {

    portENTER_CRITICAL(&mux);

    if (count >= CAPACITY) {
        droppedEvents++;   // track loss
        portEXIT_CRITICAL(&mux);
        return;
    }

    buffer[tail] = {
        type,
        value,
        0,
        millis()
    };

    tail = (tail + 1) % CAPACITY;
    count++;

    portEXIT_CRITICAL(&mux);
}

bool EventQueue::pop(Event &outEvent) {

    portENTER_CRITICAL(&mux);

    if (count == 0) {
        portEXIT_CRITICAL(&mux);
        return false;
    }

    outEvent = buffer[head];

    head = (head + 1) % CAPACITY;
    count--;

    portEXIT_CRITICAL(&mux);

    return true;
}

bool EventQueue::isEmpty() {

    portENTER_CRITICAL(&mux);

    bool empty = (count == 0);

    portEXIT_CRITICAL(&mux);

    return empty;
}
