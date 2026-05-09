#include "event_system.h"

// Push event into ring buffer
void EventQueue::push(EventType type, int value) {
    if (count >= ظرفیت) return; // drop oldest if full (safe fail)

    buffer[tail] = {
        type,
        value,
        millis()
    };

    tail = (tail + 1) % ظرفیت;
    count++;
}

// Pop event
bool EventQueue::pop(Event &outEvent) {
    if (count == 0) return false;

    outEvent = buffer[head];
    head = (head + 1) % ظرفیت;
    count--;
    return true;
}

bool EventQueue::isEmpty() {
    return count == 0;
}
