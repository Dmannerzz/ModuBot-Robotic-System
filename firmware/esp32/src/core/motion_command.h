#pragma once
#include <stdint.h>

enum class MotionAction {
    STOP = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct MotionCommand {
    MotionAction action;
    uint16_t speed;
};
