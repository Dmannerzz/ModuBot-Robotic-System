#pragma once
#include "event_system.h"

class ObstacleSystem {
public:
    void begin(EventQueue* queue);
    void update();

private:
    EventQueue* eventQueue;

    int lastDistance = 100;
    bool obstacleState = false;

    int readDistance();
    bool isValidReading(int d);

    // filtering
    int smoothDistance(int newReading);
    int buffer[5] = {100,100,100,100,100};
    int index = 0;
};
