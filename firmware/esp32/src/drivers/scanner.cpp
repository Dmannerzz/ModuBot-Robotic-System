#include "scanner.h"

void Scanner::begin() {
    servo.attach(SCANNER_SERVO_PIN);
    servo.write(SCANNER_CENTER);
    delay(300);
    
    ultrasonic.begin();
    
    Serial.println("Scanner initialized");
}

int Scanner::readDistanceAtAngle(int angle) {
    servo.write(angle);
    delay(SCAN_STEP_DELAY);
    
    int distance = ultrasonic.readDistance();
    
    if (distance <= 0 || distance >= 400) {
        return 5;
    }
    
    return distance;
}

ScanResult Scanner::performScan() {
    ScanResult result;
    
    result.leftDistance = readDistanceAtAngle(SCANNER_LEFT);
    delay(50);
    
    result.centerDistance = readDistanceAtAngle(SCANNER_CENTER);
    delay(50);
    
    result.rightDistance = readDistanceAtAngle(SCANNER_RIGHT);
    delay(50);
    
    servo.write(SCANNER_CENTER);
    
    Serial.print("Scan Results - Left: ");
    Serial.print(result.leftDistance);
    Serial.print("cm, Center: ");
    Serial.print(result.centerDistance);
    Serial.print("cm, Right: ");
    Serial.print(result.rightDistance);
    Serial.println("cm");
    
    return result;
}
