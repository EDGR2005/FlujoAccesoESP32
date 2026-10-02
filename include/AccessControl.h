#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <Arduino.h>
#include "USBKeyboardReader.h"
#include "HTTPClientManager.h"
#include "DoorController.h"

class AccessControl {
private:
    USBKeyboardReader& reader;
    HTTPClientManager& httpClient;
    DoorController& doorController;

public:
    AccessControl(USBKeyboardReader& r, HTTPClientManager& http, DoorController& dc);
    void begin();
    void process();
};

#endif