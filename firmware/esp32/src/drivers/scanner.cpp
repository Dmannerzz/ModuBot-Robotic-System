#include "scanner.h"

void Scanner::begin(Ultrasonic* sharedUltrasonic) {

    ultrasonic = sharedUltrasonic;

    servo.setPeriodHertz(50);
    servo.attach(SCANNER_SERVO_PIN, 500, 2400);

    servo.write(SCANNER_CENTER);

    state = IDLE;

    Serial.println("[Scanner] Non-blocking mode ready");
}

void Scanner::startScan() {

    // Only allow scan start from IDLE (not DONE)
    // Once scan completes and result is consumed via getResult(), state returns to IDLE
    // This prevents re-entry while result is still being processed
    if (state != IDLE) return;

    result = ScanResult();

    state = MOVE_LEFT;

    Serial.println("[Scanner] SCAN STARTED");
}

void Scanner::update() {

    unsigned long now = millis();

    switch (state) {

        case IDLE:
            break;

        // =========================
        // LEFT MOVE
        // =========================
        case MOVE_LEFT:
            servo.write(SCANNER_LEFT);
            state = READ_LEFT;
            lastActionTime = now;   // FIXED POSITION
            Serial.println("[Scanner] MOVING LEFT");
            break;

        case READ_LEFT:
            if (now - lastActionTime >= 1000) {
                if (ultrasonic) {
                    result.leftDistance = ultrasonic->readRawDistance();
                }
                state = MOVE_CENTER;
            }
            Serial.print("[Scanner] READING LEFT: ");
            Serial.println(result.leftDistance);
            break;

        // =========================
        // CENTER MOVE
        // =========================
        case MOVE_CENTER:
            servo.write(SCANNER_CENTER);
            state = READ_CENTER;
            lastActionTime = now;   // FIXED POSITION
            Serial.println("[Scanner] MOVING CENTER");
            break;

        case READ_CENTER:
            if (now - lastActionTime >= 1000) {
                if (ultrasonic) {
                    result.centerDistance = ultrasonic->readRawDistance();
                }
                state = MOVE_RIGHT;
            }
            Serial.print("[Scanner] READING CENTER: ");
            Serial.println(result.centerDistance);
            break;

        // =========================
        // RIGHT MOVE
        // =========================
        case MOVE_RIGHT:
            servo.write(SCANNER_RIGHT);
            state = READ_RIGHT;
            lastActionTime = now;   // FIXED POSITION
            Serial.println("[Scanner] MOVING RIGHT");
            break;

        case READ_RIGHT:
            if (now - lastActionTime >= 1000) {
                if (ultrasonic) {
                    result.rightDistance = ultrasonic->readRawDistance();
                }

                servo.write(SCANNER_CENTER);
                lastActionTime = now;
                state = RETURN_CENTER;
            }
            Serial.print("[Scanner] READING RIGHT: ");
            Serial.println(result.rightDistance);
            break;

        case RETURN_CENTER:
            if (now - lastActionTime >= 300) {  // servo settle time
                state = DONE;
            }
            Serial.println("[Scanner] RETURNING TO CENTER");
            break;

        case DONE:
            Serial.println("Scan done!");
            break;
    }
}

bool Scanner::isScanComplete() {
    return state == DONE;
}

ScanResult Scanner::getResult() {

    ScanResult temp = result;

    // reset properly so next scan can start clean
    state = IDLE;

    return temp;
}

void Scanner::cancelScan() {
    state = IDLE;
    servo.write(SCANNER_CENTER);
    Serial.println("[Scanner] SCAN CANCELED");
}

int Scanner::readRawDistance() {

    if (!ultrasonic) return 0;

    int d = ultrasonic->readRawDistance();

    if (d <= 0 || d > 400) return 5;

    return d;
}

