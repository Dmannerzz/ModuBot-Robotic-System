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
    yaw = 0;
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

    Serial.print("IMU offset Z: ");
    Serial.println(offsetZ);
}

void IMU::update() {

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0f;

    lastTime = now;

    // ==========================
    // SAFETY: prevent dt spikes
    // ==========================
    if (dt <= 0 || dt > 0.1f) {
        dt = 0.01f;
    }

    // gyro Z (rad/s from library)
    float rawGyroZ = g.gyro.z;

    gyroZ = rawGyroZ - offsetZ;

    // ==========================
    // CONVERT rad/s → deg/s
    // ==========================
    float gyroDeg = gyroZ * 57.2958f;

    // integrate yaw
    yaw += gyroDeg * dt;

    // normalize
    if (yaw > 180.0f) yaw -= 360.0f;
    if (yaw < -180.0f) yaw += 360.0f;
}

float IMU::getYaw() const {
    return yaw;
}

float IMU::getGyroZ() const {
    return gyroZ;
}
