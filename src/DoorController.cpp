#include "DoorController.h"

DoorController::DoorController() : isUnlocked(false) {}

void DoorController::begin() {
    ESP32PWM::allocateTimer(0);
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, 500, 2400);
    lock();
}

void DoorController::unlock() {
    servo.write(POS_UNLOCKED);
    isUnlocked = true;
}

void DoorController::lock() {
    servo.write(POS_LOCKED);
    isUnlocked = false;
}