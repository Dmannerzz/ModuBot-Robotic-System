#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>
#include "ultrasonic.h"

#define SCANNER_SERVO_PIN 32

#define SCANNER_CENTER 90
#define SCANNER_LEFT 150
#define SCANNER_RIGHT 30

constexpr int SAFE_CLEARANCE_CM = 25;

struct ScanResult {
    int leftDistance = 0;
    int centerDistance = 0;
    int rightDistance = 0;

    int getBestDirection() const {
        long left = (leftDistance >= SAFE_CLEARANCE_CM) ? leftDistance : -1;
        long center = (centerDistance >= SAFE_CLEARANCE_CM) ? centerDistance : -1;
        long right = (rightDistance >= SAFE_CLEARANCE_CM) ? rightDistance : -1;

        if (left == -1 && center == -1 && right == -1) {
            return -1;
        }

        int bestIndex = -1;
        long maxDist = -1;

        if (left > maxDist) { maxDist = left; bestIndex = 0; }
        if (center > maxDist) { maxDist = center; bestIndex = 1; }
        if (right > maxDist) { maxDist = right; bestIndex = 2; }

        return bestIndex;
    }
};

enum ScanState {
    IDLE,
    MOVE_LEFT,
    READ_LEFT,
    MOVE_CENTER,
    READ_CENTER,
    MOVE_RIGHT,
    READ_RIGHT,
    RETURN_CENTER,
    DONE
};

class Scanner {
public:
    void begin(Ultrasonic* sharedUltrasonic);

    void update();
    bool isScanComplete();
    ScanResult getResult();
    void startScan();
    void cancelScan();

private:
    Servo servo;
    Ultrasonic* ultrasonic = nullptr;

    ScanState state = IDLE;
    unsigned long lastActionTime = 0;

    ScanResult result;

    int readRawDistance();
    void moveServo(int angle);
};