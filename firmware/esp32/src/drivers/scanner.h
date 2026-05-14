#pragma once

#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif
#include "ultrasonic.h"

#define SCANNER_SERVO_PIN 32
#define SCANNER_CENTER 90
#define SCANNER_LEFT 10
#define SCANNER_RIGHT 170
#define SCAN_STEP_DELAY 100

struct ScanResult {
    int leftDistance;
    int centerDistance;
    int rightDistance;
    
    int getBestDirection() const {
        int maxDist = centerDistance;
        int direction = 0;
        
        if (leftDistance > maxDist) {
            maxDist = leftDistance;
            direction = -1;
        }
        
        if (rightDistance > maxDist) {
            maxDist = rightDistance;
            direction = 1;
        }
        
        return direction;
    }
};

class Scanner {
public:
    void begin();
    ScanResult performScan();
    
private:
    Servo servo;
    Ultrasonic ultrasonic;
    
    int readDistanceAtAngle(int angle);
};
