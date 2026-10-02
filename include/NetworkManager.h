

// include/NetworkManager.h
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include "Config.h"

class NetworkManager {
public:
    void begin();
    void keepAlive();
};

#endif