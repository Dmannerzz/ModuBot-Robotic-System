#pragma once
#include <Arduino.h>

class IMU {
public:
    void begin();

    // call frequently in loop
    void update();

    float getYaw() const;

private:
    float yaw = 0;

    // simulation fallback / placeholder filter
    float gyroZ = 0;
    unsigned long lastTime = 0;
};
