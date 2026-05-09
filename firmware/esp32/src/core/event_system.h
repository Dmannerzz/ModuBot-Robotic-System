#pragma once
#include <stdint.h>

// ==========================
// Event Types
// ==========================
enum class EventType {
    NONE = 0,

    // Movement commands
    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    STOP,

    // Mode control
    MODE_MANUAL,
    MODE_OBSTACLE,
    MODE_PATROL,
    MODE_IDLE,

    // Sensors
    OBSTACLE_DETECTED,
    OBSTACLE_CLEARED,
    LOW_LIGHT,
    SENSOR_UPDATE,

    // System
    LOG_ROUTE,
    RESET_LOGS,

    // Remote inputs
    IR_COMMAND,
    WIFI_COMMAND,

    // Internal
    TIMER_TICK
};

// ==========================
// Event Structure
// ==========================
struct Event {
    EventType type;
    int value;              // optional payload (distance, angle, etc.)
    unsigned long timestamp;
};

// ==========================
// Event Buffer (Ring Queue)
// ==========================
class EventQueue {
public:
    static const int CAPACITY = 20; // small fixed buffer for ESP32 stability

    void push(EventType type, int value = 0);
    bool pop(Event &outEvent);
    bool isEmpty();

private:
    Event buffer[CAPACITY];
    int head = 0;
    int tail = 0;
    int count = 0;
};
