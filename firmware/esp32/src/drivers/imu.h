#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class IMU {
public:
    void begin();
    void update();

    float getYaw() const;
    float getGyroZ() const;

private:
    Adafruit_MPU6050 mpu;

    float yaw = 0;
    float gyroZ = 0;

    float offsetZ = 0;

    unsigned long lastTime = 0;

    void calibrate();
};
