#include "route_storage.h"

void RouteStorage::begin() {
    if (!SD.begin(15)) {  // CS pin 15
        Serial.println("SD Card initialization failed!");
        return;
    }
    
    // Create routes directory if it doesn't exist
    if (!SD.exists(ROUTES_DIR)) {
        SD.mkdir(ROUTES_DIR);
    }
    
    Serial.println("SD Card initialized successfully");
}

String RouteStorage::getRouteFilePath(int routeIndex) {
    if (routeIndex < 0 || routeIndex >= MAX_ROUTES) {
        return "";
    }
    return String(ROUTE_FILE_PREFIX) + String(routeIndex) + ".bin";
}

void RouteStorage::startRecording(int routeIndex) {
    if (routeIndex < 0 || routeIndex >= MAX_ROUTES) {
        Serial.println("Invalid route index");
        return;
    }
    
    if (recordingRoute != -1) {
        stopRecording();
    }
    
    String filePath = getRouteFilePath(routeIndex);
    
    // Delete existing file
    if (SD.exists(filePath)) {
        SD.remove(filePath);
    }
    
    recordingFile = SD.open(filePath, FILE_WRITE);
    if (!recordingFile) {
        Serial.println("Failed to open route file for writing");
        return;
    }
    
    recordingRoute = routeIndex;
    lastTimestamp = millis();
    lastCmd = { MotionAction::STOP, 0 };
    stepCount = 0;
    
    Serial.print("Recording route ");
    Serial.print(routeIndex);
    Serial.println(" to SD card");
}

void RouteStorage::stopRecording() {
    if (recordingRoute == -1 || !recordingFile) {
        return;
    }
    
    // Write final step
    if (lastCmd.action != MotionAction::STOP) {
        RouteStep finalStep = {
            lastCmd,
            millis() - lastTimestamp,
            0.0f
        };
        writeStepToFile(finalStep);
    }
    
    recordingFile.close();
    
    Serial.print("Recording stopped. Steps saved: ");
    Serial.println(stepCount);
    
    recordingRoute = -1;
}

void RouteStorage::logCommand(const MotionCommand& cmd, float yaw) {
    if (recordingRoute == -1 || !recordingFile) {
        return;
    }
    
    // Ignore duplicates
    if (cmd.action == lastCmd.action && cmd.speed == lastCmd.speed) {
        return;
    }
    
    // Write previous step
    if (lastCmd.action != MotionAction::STOP) {
        RouteStep step = {
            lastCmd,
            millis() - lastTimestamp,
            yaw
        };
        writeStepToFile(step);
        stepCount++;
    }
    
    lastCmd = cmd;
    lastTimestamp = millis();
}

void RouteStorage::writeStepToFile(const RouteStep& step) {
    if (!recordingFile) return;
    
    // Write motion command action (1 byte)
    recordingFile.write((uint8_t)step.cmd.action);
    
    // Write motion command speed (2 bytes, little-endian)
    recordingFile.write((uint8_t)(step.cmd.speed & 0xFF));
    recordingFile.write((uint8_t)((step.cmd.speed >> 8) & 0xFF));
    
    // Write duration (4 bytes, little-endian)
    recordingFile.write((uint8_t)(step.duration & 0xFF));
    recordingFile.write((uint8_t)((step.duration >> 8) & 0xFF));
    recordingFile.write((uint8_t)((step.duration >> 16) & 0xFF));
    recordingFile.write((uint8_t)((step.duration >> 24) & 0xFF));
    
    // Write yaw (4 bytes as float)
    byte* yawBytes = (byte*)&step.yaw;
    for (int i = 0; i < 4; i++) {
        recordingFile.write(yawBytes[i]);
    }
    
    recordingFile.flush();
}

RouteStep RouteStorage::readStepFromFile(File& file) {
    RouteStep step = { {MotionAction::STOP, 0}, 0, 0.0f };
    
    if (!file || file.available() < 11) {  // 1 + 2 + 4 + 4 bytes
        return step;
    }
    
    // Read motion action
    uint8_t action = file.read();
    step.cmd.action = (MotionAction)action;
    
    // Read speed (little-endian)
    uint8_t speedLow = file.read();
    uint8_t speedHigh = file.read();
    step.cmd.speed = (speedHigh << 8) | speedLow;
    
    // Read duration (little-endian)
    uint8_t dur0 = file.read();
    uint8_t dur1 = file.read();
    uint8_t dur2 = file.read();
    uint8_t dur3 = file.read();
    step.duration = (dur3 << 24) | (dur2 << 16) | (dur1 << 8) | dur0;
    
    // Read yaw
    byte yawBytes[4];
    for (int i = 0; i < 4; i++) {
        yawBytes[i] = file.read();
    }
    step.yaw = *(float*)yawBytes;
    
    return step;
}

RouteStep RouteStorage::getStep(int routeIndex, int stepIndex) {
    RouteStep step = { {MotionAction::STOP, 0}, 0, 0.0f };
    
    String filePath = getRouteFilePath(routeIndex);
    if (!SD.exists(filePath)) {
        return step;
    }
    
    File file = SD.open(filePath, FILE_READ);
    if (!file) {
        return step;
    }
    
    // Skip to desired step
    for (int i = 0; i < stepIndex; i++) {
        if (file.available() < 11) break;
        file.seek(file.position() + 11);  // Each step is 11 bytes
    }
    
    step = readStepFromFile(file);
    file.close();
    
    return step;
}

int RouteStorage::getStepCount(int routeIndex) {
    String filePath = getRouteFilePath(routeIndex);
    if (!SD.exists(filePath)) {
        return 0;
    }
    
    File file = SD.open(filePath, FILE_READ);
    if (!file) {
        return 0;
    }
    
    int count = file.size() / 11;  // Each step is 11 bytes
    file.close();
    
    return count;
}

void RouteStorage::clearRoute(int routeIndex) {
    if (recordingRoute == routeIndex) {
        stopRecording();
    }
    
    String filePath = getRouteFilePath(routeIndex);
    if (SD.exists(filePath)) {
        SD.remove(filePath);
        Serial.print("Route ");
        Serial.print(routeIndex);
        Serial.println(" cleared");
    }
}

void RouteStorage::deleteAllRoutes() {
    for (int i = 0; i < MAX_ROUTES; i++) {
        clearRoute(i);
    }
}

bool RouteStorage::routeExists(int routeIndex) {
    String filePath = getRouteFilePath(routeIndex);
    return SD.exists(filePath) && getStepCount(routeIndex) > 0;
}

bool RouteStorage::isRecording() {
    return recordingRoute != -1;
}
