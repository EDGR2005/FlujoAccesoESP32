#ifndef USBHOST_HID_KEYBOARD_H
#define USBHOST_HID_KEYBOARD_H

#include <Arduino.h>

// Minimal stub for projects without the USBHost HID keyboard library installed.
// Provides the API used in src/main.cpp: onKey(callback), begin(), task().

class USBHostHIDkeyboard {
public:
    using KeyCallback = void(*)(uint8_t key, uint8_t mod);

    USBHostHIDkeyboard() : cb(nullptr) {}

    void onKey(KeyCallback callback) { cb = callback; }
    void begin() { /* stub: no-op */ }
    void task() { /* stub: no-op */ }

private:
    KeyCallback cb;
};

#endif
