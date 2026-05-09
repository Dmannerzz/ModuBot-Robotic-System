#include "event_system.h"

// Push event into ring buffer
void EventQueue::push(EventType type, int value) {
    if (count >= CAPACITY) return; // drop event if full (safe fail for ESP32)

    buffer[tail] = {
        type,
        value,
        millis()
    };

    tail = (tail + 1) % CAPACITY;
    count++;
}

// Pop event
bool EventQueue::pop(Event &outEvent) {
    if (count == 0) return false;

    outEvent = buffer[head];
    head = (head + 1) % CAPACITY;
    count--;
    return true;
}

bool EventQueue::isEmpty() {
    return count == 0;
}
