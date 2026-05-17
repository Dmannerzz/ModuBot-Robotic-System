#pragma once

#include "../core/event_system.h"
#include "../drivers/scanner.h"

class ObstacleSystem {
public:
    void begin(EventQueue* queue);
    void update();

    void startScan();   // async trigger
    void cancelScan();  // abort active scan

private:
    EventQueue* eventQueue;

    Scanner scanner;

    int lastDistance = 100;
    bool obstacleState = false;

    // filtering
    int smoothDistance(int newReading);
    int buffer[5] = {100,100,100,100,100};
    int index = 0;

    bool isValidReading(int d);

    long getDistance();

    // scan state
    bool scanActive = false;
    unsigned long lastScanTime = 0;
};
