#include "control_policy.h"

// ==========================
// SET AUTHORITY
// ==========================
void ControlPolicy::setAuthority(
    ControlAuthority auth
) {
    currentAuthority = auth;
}

// ==========================
// GET AUTHORITY
// ==========================
ControlAuthority ControlPolicy::getAuthority() {
    return currentAuthority;
}

// ==========================
// MOVEMENT PERMISSION
// ==========================
bool ControlPolicy::canMove(
    ControlAuthority requester
) {

    if (emergencyStopActive) {
        return false;
    }

    return requester == currentAuthority;
}

// ==========================
// EMERGENCY STOP
// ==========================
void ControlPolicy::emergencyStop() {

    emergencyStopActive = true;
}

// ==========================
// STATUS
// ==========================
bool ControlPolicy::isEmergencyStopped() {

    return emergencyStopActive;
}
