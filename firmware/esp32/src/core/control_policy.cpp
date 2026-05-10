#include "control_policy.h"

// ==========================
// SET AUTHORITY
// ==========================
void ControlPolicy::setAuthority(ControlAuthority auth) {

    currentAuthority = auth;

    // Auto-clear emergency on valid mode switch
    // (prevents system getting stuck in safety lock)
    if (auth != ControlAuthority::NONE) {
        emergencyStopActive = false;
    }
}

// ==========================
// GET AUTHORITY
// ==========================
ControlAuthority ControlPolicy::getAuthority() {
    return currentAuthority;
}

// ==========================
// MOVEMENT PERMISSION (STRICT MODE)
// ==========================
bool ControlPolicy::canMove(ControlAuthority requester) {

    // HARD SAFETY LOCK
    if (emergencyStopActive) {
        return false;
    }

    // STRICT MODE MATCHING
    // Only active mode is allowed to act
    return requester == currentAuthority;
}

// ==========================
// EMERGENCY STOP (GLOBAL LOCK)
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
// STATUS CHECK
// ==========================
bool ControlPolicy::isEmergencyStopped() {
    return emergencyStopActive;
}
