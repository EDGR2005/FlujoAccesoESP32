#include "USBKeyboardReader.h"

USBKeyboardReader::USBKeyboardReader() : inputBuffer(""), lastUID(""), cardReady(false) {}

void USBKeyboardReader::begin() {
    inputBuffer.reserve(32);
}

void USBKeyboardReader::handleKeyPress(char key) {
    if (key == '\r' || key == '\n') {
        if (inputBuffer.length() > 0) {
            lastUID = inputBuffer;
            lastUID.toUpperCase();
            lastUID.trim();
            cardReady = true;
            inputBuffer = "";
        }
    } else if (isAlphaNumeric(key)) {
        inputBuffer += key;
    }
}

bool USBKeyboardReader::isCardAvailable() {
    return cardReady;
}

String USBKeyboardReader::getUID() {
    cardReady = false;
    return lastUID;
}