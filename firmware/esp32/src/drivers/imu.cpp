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

    yaw = 0.0f;
    gyroZ = 0.0f;
    lastUpdate = millis();

    Serial.println("IMU initialized");
}

// ==========================
// CALIBRATION
// ==========================
void IMU::calibrate() {

    sensors_event_t a, g, temp;

    float sum = 0.0f;

    for (int i = 0; i < 200; i++) {

        mpu.getEvent(&a, &g, &temp);

        sum += g.gyro.z;

        delay(5);
    }

    gyroZOffset = sum / 200.0f;

    Serial.print("Gyro Z Offset: ");
    Serial.println(gyroZOffset);
}

// ==========================
// UPDATE LOOP (CORE IMU ENGINE)
// ==========================
void IMU::update() {

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    unsigned long now = millis();
    float dt = (now - lastUpdate) / 1000.0f;

    lastUpdate = now;

    // ==========================
    // protect against bad dt
    // ==========================
    if (dt <= 0.0f || dt > 0.1f) {
        dt = 0.01f;
    }

    // raw gyro (rad/s)
    float rawGyroZ = g.gyro.z;

    // apply calibration offset
    gyroZ = rawGyroZ - gyroZOffset;

    // convert rad/s → deg/s
    float gyroDeg = gyroZ * 57.2958f;

    // integrate yaw
    yaw += gyroDeg * dt;

    // normalize angle
    if (yaw > 180.0f) yaw -= 360.0f;
    if (yaw < -180.0f) yaw += 360.0f;
}

// ==========================
// OUTPUT
// ==========================
float IMU::getYaw() {
    return yaw;
}
