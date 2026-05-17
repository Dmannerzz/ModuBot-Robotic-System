#include "ultrasonic.h"
#include <Arduino.h>

#define TRIG_PIN 26
#define ECHO_PIN 36

// ==========================
// INIT
// ==========================
void Ultrasonic::begin() {

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(TRIG_PIN, LOW);

    // create mutex
    if (mutex == nullptr) {
        mutex = xSemaphoreCreateMutex();
    }

    // initialize filtered Distance
    filteredDistance = 0;
}

// ==========================
// READ DISTANCE (immediate, protected)
// ==========================
int Ultrasonic::readRawDistance() {

    // =========================
    // SENSOR LOCK
    // =========================
    if (mutex) {
        xSemaphoreTake(mutex, portMAX_DELAY);
    }

    // =========================
    // TRIGGER PULSE
    // =========================
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    // =========================
    // READ ECHO
    // =========================
    long duration = pulseIn(ECHO_PIN, HIGH, 50000);

    int rawDistance = 0;

    if (duration > 0) {
        rawDistance = (int)((duration * 0.0343f) / 2.0f);
    }

    // =========================
    // RELEASE LOCK
    // =========================
    if (mutex) {
        xSemaphoreGive(mutex);
    }

    return rawDistance;
}

// ==========================
// BACKGROUND TASK
// ==========================
void Ultrasonic::startBackgroundTask(unsigned long sampleMs) {
    sampleIntervalMs = sampleMs;
    if (taskHandle != nullptr) return; // already running

    if (mutex == nullptr) {
        mutex = xSemaphoreCreateMutex();
    }

    xTaskCreatePinnedToCore(Ultrasonic::ultrasonicTask, "ultraTask", 2048, this, 1, &taskHandle, 1);
}

void Ultrasonic::ultrasonicTask(void* param) {
    Ultrasonic* self = reinterpret_cast<Ultrasonic*>(param);

    for (;;) {
        if (self->mutex) xSemaphoreTake(self->mutex, portMAX_DELAY);

        // trigger pulse
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        long duration = pulseIn(ECHO_PIN, HIGH, 50000);
        int distance = self->lastValidDistance;

        if (duration > 0) {
            int d = (int)(duration * 0.034f / 2.0f);
            if (d >= 2 && d <= 400) {
                distance = (int)((self->lastValidDistance * 0.7f) + (d * 0.3f));
                self->lastValidDistance = distance;
            }
        }

        self->filteredDistance = distance;

        if (self->mutex) xSemaphoreGive(self->mutex);

        vTaskDelay(pdMS_TO_TICKS(self->sampleIntervalMs));
    }
}

int Ultrasonic::getFilteredDistance() {
    int d = filteredDistance;
    if (mutex) {
        if (xSemaphoreTake(mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            d = filteredDistance;
            xSemaphoreGive(mutex);
        }
    }
    return d;
}
