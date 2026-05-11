#include "imu.h"

void IMU::begin() {

    if (!mpu.begin()) {
        Serial.println("MPU6050 not found!");
        while (1);
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    calibrate();

    lastTime = millis();
}

void IMU::calibrate() {

    sensors_event_t a, g, temp;

    float sum = 0;

    for (int i = 0; i < 200; i++) {
        mpu.getEvent(&a, &g, &temp);
        sum += g.gyro.z;
        delay(5);
    }

    offsetZ = sum / 200.0f;
}

void IMU::update() {

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0f;
    lastTime = now;

    gyroZ = g.gyro.z - offsetZ;

    // integrate to get yaw
    yaw += gyroZ * dt;

    // normalize
    if (yaw > 180) yaw -= 360;
    if (yaw < -180) yaw += 360;
}

float IMU::getYaw() const {
    return yaw;
}

float IMU::getGyroZ() const {
    return gyroZ;
}
