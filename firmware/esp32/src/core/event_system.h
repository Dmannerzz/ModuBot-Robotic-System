#pragma once
#include <stdint.h>
#include "esp32-hal.h"

enum class EventType {
    NONE = 0,

    MOVE_FORWARD,
    MOVE_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    STOP,

    MODE_MANUAL,
    MODE_OBSTACLE,
    MODE_PATROL,
    MODE_IDLE,

    OBSTACLE_DETECTED,
    OBSTACLE_CLEARED,
    LOW_LIGHT,
    SENSOR_UPDATE,

    LOG_ROUTE,
    RESET_LOGS,
    
    SELECT_ROUTE_0,
    SELECT_ROUTE_1,

    IR_COMMAND,
    WIFI_COMMAND,

    TIMER_TICK
};

struct Event {
    EventType type;
    int value;
    int value2 = 0;   // ✅ ADDED FOR SAFETY
    unsigned long timestamp;
};

class EventQueue {
public:
    static const int CAPACITY = 20;

    void push(EventType type, int value = 0);
    bool pop(Event &outEvent);
    bool isEmpty();

private:
    Event buffer[CAPACITY];

    int head = 0;
    int tail = 0;
    int count = 0;

    volatile int droppedEvents = 0; // ✅ ADDED

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};
