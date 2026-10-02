#ifndef HTTP_CLIENT_MANAGER_H
#define HTTP_CLIENT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"

class HTTPClientManager {
private:
    String serverUrl;

public:
    HTTPClientManager();
    void begin();
    bool verifyAccess(const String& uid);
};

#endif
