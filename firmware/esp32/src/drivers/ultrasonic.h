#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class Ultrasonic {
public:
    void begin();

    // Immediate read (locks sensor to perform a fresh pulse)
    int readRawDistance();

    // Start a background sampling task that updates getFilteredDistance()
    void startBackgroundTask(unsigned long sampleMs = 30);

    // Non-blocking-ish read of the last sampled value (fast)
    int getFilteredDistance();

private:
    int lastValidDistance = 0;
    volatile int filteredDistance = 0;
    SemaphoreHandle_t mutex = nullptr;
    TaskHandle_t taskHandle = nullptr;
    unsigned long sampleIntervalMs = 30;

    static void ultrasonicTask(void* param);
};
