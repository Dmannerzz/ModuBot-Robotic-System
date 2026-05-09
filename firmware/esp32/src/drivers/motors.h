#pragma once

class Motors {
public:
    static void set(int left, int right);
    static void stop();
    static void turnLeft(int speed);
    static void turnRight(int speed);
};
