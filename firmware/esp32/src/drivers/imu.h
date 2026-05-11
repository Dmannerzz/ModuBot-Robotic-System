#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class IMU {
public:
    void begin();

    void update();      // refresh sensor data
    float getYaw();     // main robot heading output

private:
    Adafruit_MPU6050 mpu;

    float yaw = 0.0f;
    float gyroZOffset = 0.0f;

    float gyroZ = 0.0f;

    unsigned long lastUpdate = 0;

    void calibrate();
};
