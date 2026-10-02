#include "HTTPClientManager.h"

HTTPClientManager::HTTPClientManager() : serverUrl(API_CHECK_URL) {}

void HTTPClientManager::begin() {
    // nothing to initialize for now
}

bool HTTPClientManager::verifyAccess(const String& uid) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[HTTP] Error: Sin conexión Wi-Fi");
        return false; // fail closed
    }

    HTTPClient http;
    http.begin(serverUrl);
    http.setTimeout(API_TIMEOUT_MS);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<128> payloadDoc;
    payloadDoc["uid"] = uid;
    String payload;
    serializeJson(payloadDoc, payload);

    int httpResponseCode = http.POST(payload);
    bool accessGranted = false;

    if (httpResponseCode > 0) {
        String response = http.getString();
        StaticJsonDocument<256> responseDoc;
        DeserializationError err = deserializeJson(responseDoc, response);
        if (!err) {
            accessGranted = responseDoc["granted"] | false;
        } else {
            Serial.println("[HTTP] Error parseando respuesta JSON");
        }
    } else {
        Serial.printf("[HTTP] Error en petición POST: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
    return accessGranted;
}
