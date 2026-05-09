#include "obstacle_system.h"
#include "ultrasonic.h"

static Ultrasonic ultrasonic;

// ==========================
// INIT
// ==========================
void ObstacleSystem::begin(EventQueue* queue) {

    eventQueue = queue;

    ultrasonic.begin();
}
