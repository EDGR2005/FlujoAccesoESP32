# ESP32 NFC Access Control

## Descripción general

Este proyecto implementa un sistema de control de acceso con ESP32, lector RFID RC522 y validación de permisos a través de un backend externo.

La lógica principal se ejecuta en `src/main.cpp` y está orientada a:
- detectar una tarjeta RFID
- leer su UID
- enviar el UID al backend por HTTP
- decidir si abre o deniega el acceso
- activar LEDs, buzzer y servomotor

## Arquitectura

- ESP32 + RC522: lectura del identificador único de la tarjeta
- Wi‑Fi: conexión a la red local
- Backend externo: revisión de autenticación y autorización (LAMP: PHP + SQLite3)
- Servo: apertura de la puerta
- LEDs: indicador visual de acceso permitido/denegado
- Buzzer: alerta auditiva del resultado

> 📊 **Diagramas de Flujo y Arquitectura:** Consulta la documentación completa con diagramas Mermaid en [FLUJO_SISTEMA.md](FLUJO_SISTEMA.md).

## Hardware actual

### RC522 SPI

- RST -> GPIO 9
- SS -> GPIO 10
- MOSI -> GPIO 11
- MISO -> GPIO 13
- SCK -> GPIO 12

### Servo

- Servo -> GPIO 18

### Indicadores y alarma

- LED acceso permitido -> GPIO 25
- LED acceso denegado -> GPIO 26
- Buzzer -> GPIO 27

## Configuración

La configuración está en `include/Config.h`.

Se deben definir:
- `WIFI_SSID`
- `WIFI_PASSWORD`
- `API_CHECK_URL`
- `API_TIMEOUT_MS`
- pines del módulo RC522
- pines de LEDs, servo y buzzer

Ejemplo:

```cpp
#define WIFI_SSID "TU_SSID"
#define WIFI_PASSWORD "TU_PASSWORD"
#define API_CHECK_URL "http://192.168.1.98/api/check.php"
#define API_TIMEOUT_MS 3000

#define RST_PIN 9
#define SS_PIN 10
#define MOSI_PIN 11
#define MISO_PIN 13
#define SCK_PIN 12

#define SERVO_PIN 18
#define ALLOWED_LED_PIN 25
#define DENIED_LED_PIN 26
#define BUZZER_PIN 27
```

## Flujo de acceso

1. El ESP32 se conecta a Wi‑Fi.
2. El módulo RC522 espera una tarjeta.
3. Cuando detecta una tarjeta, lee el UID.
4. Envía un JSON al backend:

```json
{ "uid": "A1B2C3D4" }
```

5. El backend responde con JSON similar a:

```json
{
  "granted": true,
  "is_admin": false,
  "message": "Acceso concedido"
}
```

6. El ESP32:
   - activa el LED de acceso permitido o denegado
   - emite el patrón de sonido correspondiente
   - si el acceso es permitido, abre el servomotor durante 2 segundos
   - luego cierra la puerta

## Comportamiento del buzzer

- Acceso permitido: 1 tono breve
- Acceso denegado: 2 tonos cortos

## Comportamiento del servomotor

- Estado cerrado: `POS_LOCKED`
- Estado abierto: `POS_UNLOCKED`
- Duración abierta: `DOOR_OPEN_TIME_MS = 2000`

## Backend esperado

El backend debe aceptar una petición HTTP POST con JSON y responder un JSON con al menos:

```json
{ "granted": true }
```

o bien:

```json
{ "granted": false }
```

También puede incluir campos adicionales:

```json
{
  "granted": true,
  "is_admin": true,
  "message": "Acceso concedido",
  "redirect_url": "http://192.168.1.98/admin"
}
```

## Dependencias del proyecto

El proyecto usa PlatformIO con las siguientes librerías:

- `ESP32Servo`
- `ArduinoJson`
- `MFRC522`
- `WiFi`
- `HTTPClient`
- `SPI`

## Compilación

Desde la raíz del proyecto:

```bash
/home/edu-gar/.platformio/penv/bin/platformio run
```

## Carga al dispositivo

```bash
/home/edu-gar/.platformio/penv/bin/platformio run --target upload
```

## Estado actual

La versión actual del proyecto está enfocada en:
- lector RFID RC522 en SPI
- backend externo para validación
- control visual/auditivo de accesos
- uso de un único archivo principal `src/main.cpp`

## Siguientes mejoras sugeridas

- reintentos HTTP en fallos de red
- caché local para accesos recientes
- manejo de acceso admin y redirección
- HTTPS con autenticación token
- registro más detallado de accesos y errores
