#pragma once

#include "event_system.h"

enum class ControlAuthority {
    NONE = 0,
    MANUAL,
    OBSTACLE,
    PATROL
};

class ControlPolicy {
public:

    void setAuthority(ControlAuthority auth);
    ControlAuthority getAuthority();

    bool canMove(ControlAuthority requester);

    void emergencyStop();
    void resetEmergency();

    bool isEmergencyStopped();

private:

    ControlAuthority currentAuthority = ControlAuthority::NONE;

    bool emergencyStopActive = false;
};
