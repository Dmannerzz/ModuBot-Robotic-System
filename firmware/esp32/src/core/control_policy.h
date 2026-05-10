#pragma once

#include "event_system.h"

enum class ControlAuthority {
    NONE = 0,
    PATROL = 1,
    MANUAL = 2,
    OBSTACLE = 3,
    SAFETY = 4
};

class ControlPolicy {
public:

    void setAuthority(ControlAuthority auth);

    ControlAuthority getAuthority();

    bool canMove(ControlAuthority requester);

    void emergencyStop();

    void resetEmergency();   // NEW

    bool isEmergencyStopped();

private:

    ControlAuthority currentAuthority = ControlAuthority::NONE;

    bool emergencyStopActive = false;
};
