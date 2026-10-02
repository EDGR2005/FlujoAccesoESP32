#ifndef USB_KEYBOARD_READER_H
#define USB_KEYBOARD_READER_H

#include <Arduino.h>

class USBKeyboardReader {
private:
    String inputBuffer;
    String lastUID;
    bool cardReady;

public:
    USBKeyboardReader();
    void begin();
    void handleKeyPress(char key);
    bool isCardAvailable();
    String getUID();
};

#endif