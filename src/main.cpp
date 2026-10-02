#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#include "Config.h"

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo doorServo;


// ============================================================
// ULTRASONICO
// ============================================================

float readUltrasonicDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration == 0) {
        return -1;
    }

    float distance = (duration / 2.0) / 29.1;

    return distance;
}


bool isPresenceDetected() {
    float distance = readUltrasonicDistance();

    bool presence =
        distance > 0 &&
        distance < MAX_DISTANCE_CM;

    if (presence) {
        Serial.printf(
            "[SENSOR] Presencia detectada: %.1f cm\n",
            distance
        );
    }

    return presence;
}


// Muestrea varias veces para evitar lecturas falsas
bool isPresenceStable(int samples = 4, int intervalMs = 40) {

    int positive = 0;

    for (int i = 0; i < samples; ++i) {

        float d = readUltrasonicDistance();

        if (d > 0 && d < MAX_DISTANCE_CM) {
            positive++;
        }

        vTaskDelay(pdMS_TO_TICKS(intervalMs));
    }

    bool stable = positive >= (samples / 2 + 1);

    if (stable) {

        Serial.printf(
            "[SENSOR] Presencia estable (%d/%d muestras)\n",
            positive,
            samples
        );

    } else {

        Serial.printf(
            "[SENSOR] Presencia NO estable (%d/%d muestras)\n",
            positive,
            samples
        );
    }

    return stable;
}


// ============================================================
// BUZZER
// ============================================================

// Buzzer activo:
// HIGH = encendido
// LOW  = apagado

void beep(int durationMs) {

    digitalWrite(BUZZER_PIN, HIGH);

    delay(durationMs);

    digitalWrite(BUZZER_PIN, LOW);

    delay(100);
}


void soundAllowed() {

    // Un beep corto
    beep(150);
}


void soundDenied() {

    // Dos beeps
    beep(120);

    delay(80);

    beep(120);

    // Garantizar que quede apagado
    digitalWrite(BUZZER_PIN, LOW);
}


// ============================================================
// LEDS
// ============================================================

void setAccessIndicators(bool granted, bool isAdmin = false) {

    if (granted) {

        digitalWrite(ALLOWED_LED_PIN, HIGH);
        digitalWrite(DENIED_LED_PIN, LOW);

    } else {

        digitalWrite(ALLOWED_LED_PIN, LOW);
        digitalWrite(DENIED_LED_PIN, HIGH);
    }

    if (granted && isAdmin) {
        digitalWrite(ALLOWED_LED_PIN, HIGH);
    }
}


void clearAccessIndicators() {

    digitalWrite(ALLOWED_LED_PIN, LOW);
    digitalWrite(DENIED_LED_PIN, LOW);
}


// ============================================================
// WIFI
// ============================================================

void TaskNetwork(void *pvParameters) {

    Serial.println("[WIFI] Conectando a la red...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {

        vTaskDelay(pdMS_TO_TICKS(1000));

        Serial.print(".");
    }

    Serial.println();

    Serial.println("[WIFI] ¡Conectado con éxito!");

    for (;;) {

        if (WiFi.status() != WL_CONNECTED) {

            Serial.println(
                "[WIFI] Conexión perdida. Reconectando..."
            );

            WiFi.reconnect();
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}


// ============================================================
// CONTROL DE ACCESO
// ============================================================

void TaskAccessControl(void *pvParameters) {

    // --------------------------------------------------------
    // RFID
    // --------------------------------------------------------

    SPI.begin(
        SCK_PIN,
        MISO_PIN,
        MOSI_PIN,
        SS_PIN
    );

    mfrc522.PCD_Init();

    Serial.println(
        "[RFID] Módulo RC522 inicializado correctamente"
    );


    // --------------------------------------------------------
    // SERVO
    // --------------------------------------------------------

    doorServo.attach(SERVO_PIN);

    Serial.println(
        "[Servo] Adjuntando servomotor y realizando prueba"
    );

    doorServo.write(POS_LOCKED);

    delay(500);

    doorServo.write(POS_UNLOCKED);

    Serial.println(
        "[Servo] Prueba: POS_UNLOCKED"
    );

    delay(1000);

    doorServo.write(POS_LOCKED);

    Serial.println(
        "[Servo] Prueba: POS_LOCKED"
    );

    clearAccessIndicators();

    Serial.println(
        "[Servo] Inicializado en posición cerrada"
    );

    Serial.println(
        "[SENSOR] Sensor ultrasónico listo"
    );


    // ========================================================
    // LOOP DE LA TAREA
    // ========================================================

    for (;;) {

        vTaskDelay(pdMS_TO_TICKS(50));


        // ----------------------------------------------------
        // Si no hay presencia, cerrar puerta
        // ----------------------------------------------------

        if (!isPresenceDetected()) {

            doorServo.write(POS_LOCKED);

            clearAccessIndicators();
        }


        // ----------------------------------------------------
        // Verificar RC522
        // ----------------------------------------------------

        {
            uint8_t ver =
                mfrc522.PCD_ReadRegister(
                    MFRC522::VersionReg
                );

            if (ver == 0x00 || ver == 0xFF) {

                Serial.printf(
                    "[RFID] Versión inválida 0x%02X - "
                    "re-inicializando RC522\n",
                    ver
                );

                digitalWrite(RST_PIN, LOW);

                delay(50);

                digitalWrite(RST_PIN, HIGH);

                delay(50);

                mfrc522.PCD_Init();

                vTaskDelay(
                    pdMS_TO_TICKS(200)
                );

                continue;
            }
        }


        // ----------------------------------------------------
        // Detectar tarjeta
        // ----------------------------------------------------

        if (
            mfrc522.PICC_IsNewCardPresent() &&
            mfrc522.PICC_ReadCardSerial()
        ) {

            String uidString = "";


            // ------------------------------------------------
            // Convertir UID a texto
            // ------------------------------------------------

            for (
                byte i = 0;
                i < mfrc522.uid.size;
                i++
            ) {

                if (
                    mfrc522.uid.uidByte[i] < 0x10
                ) {

                    uidString += "0";
                }

                uidString += String(
                    mfrc522.uid.uidByte[i],
                    HEX
                );
            }

            uidString.toUpperCase();

            Serial.println(
                "[RFID] UID detectado: " +
                uidString
            );


            // ------------------------------------------------
            // Finalizar comunicación con tarjeta
            // ------------------------------------------------

            mfrc522.PICC_HaltA();

            mfrc522.PCD_StopCrypto1();


            // ------------------------------------------------
            // Verificar presencia estable
            // ------------------------------------------------

            if (!isPresenceStable(4, 40)) {

                Serial.println(
                    "[SENSOR] Sin presencia estable. "
                    "Ignorando tarjeta."
                );

                continue;
            }


            bool accessGranted = false;
            bool isAdmin = false;

            const char* message =
                "Acceso denegado";


            // ------------------------------------------------
            // Consultar API
            // ------------------------------------------------

            if (
                WiFi.status() ==
                WL_CONNECTED
            ) {

                HTTPClient http;

                http.begin(
                    API_CHECK_URL
                );

                http.setTimeout(
                    API_TIMEOUT_MS
                );

                http.addHeader(
                    "Content-Type",
                    "application/json"
                );


                StaticJsonDocument<128> doc;

                doc["uid"] =
                    uidString;

                String jsonPayload;

                serializeJson(
                    doc,
                    jsonPayload
                );


                int httpResponseCode =
                    http.POST(
                        jsonPayload
                    );


                // --------------------------------------------
                // Respuesta HTTP
                // --------------------------------------------

                if (
                    httpResponseCode > 0
                ) {

                    String response =
                        http.getString();


                    Serial.printf(
                        "[HTTP] Respuesta recibida "
                        "(%d bytes): %s\n",
                        response.length(),
                        response.c_str()
                    );


                    String cleanResponse =
                        response;

                    cleanResponse.trim();


                    int jsonStart =
                        cleanResponse.indexOf('{');

                    int jsonEnd =
                        cleanResponse.lastIndexOf('}');


                    if (
                        jsonStart != -1 &&
                        jsonEnd != -1 &&
                        jsonEnd > jsonStart
                    ) {

                        cleanResponse =
                            cleanResponse.substring(
                                jsonStart,
                                jsonEnd + 1
                            );
                    }


                    StaticJsonDocument<256>
                        responseDoc;


                    DeserializationError error =
                        deserializeJson(
                            responseDoc,
                            cleanResponse
                        );


                    if (!error) {

                        accessGranted =
                            responseDoc["granted"]
                            | false;

                        isAdmin =
                            responseDoc["is_admin"]
                            | false;


                        if (
                            responseDoc["message"]
                        ) {

                            message =
                                responseDoc["message"];
                        }


                        Serial.println(
                            message
                        );


                        // ------------------------------------
                        // Administrador
                        // ------------------------------------

                        if (
                            accessGranted &&
                            isAdmin &&
                            responseDoc[
                                "redirect_url"
                            ]
                        ) {

                            Serial.println(
                                "================================"
                            );

                            Serial.println(
                                "¡ACCESO DE ADMINISTRADOR!"
                            );

                            Serial.print(
                                "Panel de Registro: "
                            );

                            Serial.println(
                                responseDoc[
                                    "redirect_url"
                                ].as<const char*>()
                            );

                            Serial.println(
                                "================================"
                            );
                        }

                    } else {

                        Serial.printf(
                            "[HTTP] Error parseando JSON: %s\n",
                            error.c_str()
                        );
                    }

                } else {

                    Serial.printf(
                        "[HTTP] Error en petición POST: %s\n",
                        http.errorToString(
                            httpResponseCode
                        ).c_str()
                    );
                }


                http.end();

            } else {

                Serial.println(
                    "[HTTP] Error: Sin conexión Wi-Fi."
                );
            }


            // =================================================
            // ACCESO CONCEDIDO
            // =================================================

            if (accessGranted) {

                Serial.println(
                    "--> ACCESO CONCEDIDO"
                );


                setAccessIndicators(
                    true,
                    isAdmin
                );


                soundAllowed();


                doorServo.write(
                    POS_UNLOCKED
        
                );
                delay(3000);


                // --------------------------------------------
                // Mantener abierto mientras hay presencia
                // --------------------------------------------

                unsigned long openTime =
                    millis();

                bool presenceActive =
                    true;


                while (
                    presenceActive &&
                    (
                        millis() -
                        openTime
                    ) <
                    (
                        DOOR_OPEN_TIME_MS +
                        5000
                    )
                ) {

                    if (
                        !isPresenceDetected()
                    ) {

                        presenceActive =
                            false;
                    }

                    vTaskDelay(
                        pdMS_TO_TICKS(100)
                    );
                }


                doorServo.write(
                    POS_LOCKED
                );


                clearAccessIndicators();


                Serial.println(
                    "--> Puerta cerrada"
                );
            }


            // =================================================
            // ACCESO DENEGADO
            // =================================================

            else {

                Serial.println(
                    "--> ACCESO DENEGADO"
                );


                setAccessIndicators(
                    false,
                    false
                );


                soundDenied();


                clearAccessIndicators();
            }
        }
    }
}


// ============================================================
// SETUP
// ============================================================

void setup() {

    Serial.begin(115200);

    delay(1000);


    Serial.println();

    Serial.println(
        "--- INICIANDO SISTEMA ESP32 ---"
    );

    Serial.println(
        "RFID + SENSOR ULTRASONICO + SERVO"
    );


    // --------------------------------------------------------
    // Pines
    // --------------------------------------------------------

    pinMode(
        ALLOWED_LED_PIN,
        OUTPUT
    );

    pinMode(
        DENIED_LED_PIN,
        OUTPUT
    );

    pinMode(
        BUZZER_PIN,
        OUTPUT
    );

    pinMode(
        RST_PIN,
        OUTPUT
    );

    pinMode(
        TRIG_PIN,
        OUTPUT
    );

    pinMode(
        ECHO_PIN,
        INPUT
    );


    // --------------------------------------------------------
    // Estados iniciales
    // --------------------------------------------------------

    digitalWrite(
        ALLOWED_LED_PIN,
        LOW
    );

    digitalWrite(
        DENIED_LED_PIN,
        LOW
    );


    // IMPORTANTE:
    // buzzer activo
    // LOW = apagado
    digitalWrite(
        BUZZER_PIN,
        LOW
    );


    // RC522 fuera de reset
    digitalWrite(
        RST_PIN,
        HIGH
    );


    // Trigger ultrasónico inicialmente LOW
    digitalWrite(
        TRIG_PIN,
        LOW
    );


    // --------------------------------------------------------
    // Tareas FreeRTOS
    // --------------------------------------------------------

    xTaskCreatePinnedToCore(
        TaskNetwork,
        "NetworkTask",
        4096,
        NULL,
        1,
        NULL,
        0
    );


    xTaskCreatePinnedToCore(
        TaskAccessControl,
        "AccessTask",
        8192,
        NULL,
        2,
        NULL,
        1
    );
}


// ============================================================
// LOOP
// ============================================================

void loop() {

    vTaskDelete(NULL);
}