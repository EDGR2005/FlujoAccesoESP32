#include "AccessControl.h"

AccessControl::AccessControl(USBKeyboardReader& r, HTTPClientManager& http, DoorController& dc)
	: reader(r), httpClient(http), doorController(dc) {}

void AccessControl::begin() {
	reader.begin();
	httpClient.begin();
	doorController.begin();
}

void AccessControl::process() {
	if (reader.isCardAvailable()) {
		String uid = reader.getUID();
		Serial.println("[NFC] UID detectado: " + uid);

		bool granted = httpClient.verifyAccess(uid);

		if (granted) {
			Serial.println("--> ACCESO CONCEDIDO");
			doorController.unlock();
			vTaskDelay(pdMS_TO_TICKS(DOOR_OPEN_TIME_MS));
			doorController.lock();
		} else {
			Serial.println("--> ACCESO DENEGADO");
		}
	}
}
