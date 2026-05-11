#include "imu.h"

// ==========================
// INIT
// ==========================
void IMU::begin() {
    yaw = 0;
    lastTime = millis();
}

// ==========================
// UPDATE (placeholder gyro integration)
// ==========================
void IMU::update() {

    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0f;
    lastTime = now;

    // TEMP: no real sensor yet
    // This prevents crashes and lets architecture compile
    float simulatedRotation = 0;

    yaw += simulatedRotation * dt;
}

// ==========================
// GET YAW
// ==========================
float IMU::getYaw() const {
    return yaw;
}
