#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class IMU {
public:
    void begin();

    void update();      // refresh sensor data
    float getYaw();     // main output for robot

private:
    Adafruit_MPU6050 mpu;

    float yaw = 0;
    float gyroZOffset = 0;

    unsigned long lastUpdate = 0;

    void calibrate();
};
