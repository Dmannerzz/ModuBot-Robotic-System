#include "control_policy.h"

// ==========================
// SET AUTHORITY
// ==========================
void ControlPolicy::setAuthority(ControlAuthority auth) {
    currentAuthority = auth;
}

// ==========================
// GET AUTHORITY
// ==========================
ControlAuthority ControlPolicy::getAuthority() {
    return currentAuthority;
}

// ==========================
// PRIORITY-BASED MOVEMENT CONTROL
// ==========================
bool ControlPolicy::canMove(ControlAuthority requester) {

    if (emergencyStopActive) {
        return false;
    }

    // Higher or equal authority wins
    return (int)requester >= (int)currentAuthority;
}

// ==========================
// EMERGENCY STOP
// ==========================
void ControlPolicy::emergencyStop() {
    emergencyStopActive = true;
}

// ==========================
// RESET EMERGENCY STATE
// ==========================
void ControlPolicy::resetEmergency() {
    emergencyStopActive = false;
}

// ==========================
// STATUS
// ==========================
bool ControlPolicy::isEmergencyStopped() {
    return emergencyStopActive;
}
