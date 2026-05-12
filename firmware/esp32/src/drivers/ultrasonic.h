#pragma once

class Ultrasonic {
public:
    void begin();

    int readDistance();

private:
    int lastValidDistance = 0;
};
