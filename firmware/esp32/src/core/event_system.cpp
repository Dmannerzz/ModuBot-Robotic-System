#include "event_system.h"

// Push event into ring buffer
void EventQueue::push(EventType type, int value) {
    portENTER_CRITICAL(&mux);

    if (count >= CAPACITY) {
        portEXIT_CRITICAL(&mux);
        return; // drop event if full (safe fail for ESP32)
    }

    buffer[tail] = {
        type,
        value,
        millis()
    };

    tail = (tail + 1) % CAPACITY;
    count++;

    portEXIT_CRITICAL(&mux);
}

// Pop event
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
