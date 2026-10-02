// src/NetworkManager.cpp
#include "NetworkManager.h"

void NetworkManager::begin() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void NetworkManager::keepAlive() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
    }
}