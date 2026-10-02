#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Network
#define WIFI_SSID "A34 de Jose Eduardo"
#define WIFI_PASSWORD "1234567890"

// API REST Externa
#define API_CHECK_URL "http://10.239.100.141/api/check.php"
#define API_TIMEOUT_MS 3000

// RC522 / SPI (usando pins SPI nativos del ESP32)
#define RST_PIN 21
#define SS_PIN 5
#define MOSI_PIN 23
#define MISO_PIN 19
#define SCK_PIN 18

// Actuadores / indicadores
#define SERVO_PIN 2  // Cambié de 18 a 2 (18 ahora es SCK)
#define ALLOWED_LED_PIN 25
#define DENIED_LED_PIN 26
#define BUZZER_PIN 27

// Sensor Ultrasónico HC-SR04
#define TRIG_PIN 14  // Cambié de 21 a 14 (21 ahora es RST del RC522)
#define ECHO_PIN 15  // Cambié de 22 a 15 para mantener par
#define MAX_DISTANCE_CM 5  // Distancia máxima para detectar presencia

// Servomotor
#define POS_LOCKED 0
#define POS_UNLOCKED 90
#define DOOR_OPEN_TIME_MS 2000

#endif
