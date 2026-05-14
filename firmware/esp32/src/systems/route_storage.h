#pragma once

#include <Arduino.h>
#include <SD.h>
#include "../core/motion_command.h"

#define MAX_ROUTES 2
#define ROUTES_DIR "/routes"
#define ROUTE_FILE_PREFIX "/routes/route_"

struct RouteStep {
    MotionCommand cmd;
    uint32_t duration;
    float yaw;
};

class RouteStorage {
public:
    void begin();
    
    // Recording
    void startRecording(int routeIndex);
    void stopRecording();
    void logCommand(const MotionCommand& cmd, float yaw);
    bool isRecording();
    
    // Playback
    RouteStep getStep(int routeIndex, int stepIndex);
    int getStepCount(int routeIndex);
    
    // Management
    void clearRoute(int routeIndex);
    void deleteAllRoutes();
    bool routeExists(int routeIndex);
    
private:
    int recordingRoute = -1;
    File recordingFile;
    MotionCommand lastCmd;
    uint32_t lastTimestamp = 0;
    int stepCount = 0;
    
    String getRouteFilePath(int routeIndex);
    void writeStepToFile(const RouteStep& step);
    RouteStep readStepFromFile(File& file);
};
