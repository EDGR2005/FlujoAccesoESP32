#ifndef DOOR_CONTROLLER_H
#define DOOR_CONTROLLER_H

#include <ESP32Servo.h>
#include "Config.h"

class DoorController {
private:
    Servo servo;
    bool isUnlocked;

public:
    DoorController();
    void begin();
    void unlock();
    void lock();
    bool getState() const { return isUnlocked; }
};

#endif