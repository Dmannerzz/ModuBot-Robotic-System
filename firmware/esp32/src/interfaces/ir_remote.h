#pragma once

#include <Arduino.h>

#include "../core/event_system.h"
#include "../core/config.h"

class IRRemote {
public:

    void init(EventQueue* queue);

    void update();

private:

    EventQueue* eventQueue = nullptr;

    void handleCommand(uint32_t code);
};