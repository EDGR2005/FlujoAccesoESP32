# Conexión de hardware ESP32

## Diagrama de pines

```
ESP32 Dev Module
┌─────────────────────────────────────┐
│                                     │
│  GND  3V3  EN  SENSOR_VP SENSOR_VN  │
│   ║    ║   ║      ║         ║      │
│  ─┴─  ─┴─  ─┴─    ─┴─       ─┴─    │
│                                     │
│  D35  D34  D39   D36   D1   D3   TX │
│   ║    ║    ║     ║    ║    ║    ║ │
│  ─┴─  ─┴─  ─┴─   ─┴─  ─┴─  ─┴─  ─┴─│
│                                     │
│  GND  D5   D17   D16  D4   D2  D15  │
│   ║    ║    ║     ║    ║    ║   ║  │
│  ─┴─  ─┴─  ─┴─   ─┴─  ─┴─  ─┴─ ─┴─ │
│                                     │
│  3V3 D19  D20  D21  GND  D14  D13  D12 │
│   ║   ║    ║    ║    ║    ║    ║    ║ │
│  ─┴─ ─┴─  ─┴─  ─┴─  ─┴─  ─┴─  ─┴─  ─┴─│
│                                     │
│  D11 D10   D9   D8   D7   D6   GND  │
│   ║   ║    ║    ║    ║    ║    ║   │
│  ─┴─ ─┴─  ─┴─  ─┴─  ─┴─  ─┴─  ─┴─  │
│                                     │
└─────────────────────────────────────┘
```

## Conexión del módulo RC522

El módulo RC522 se conecta al ESP32 mediante SPI nativo.

| RC522 | ESP32 | Descripción |
|-------|-------|------------|
| VCC   | 3V3   | Alimentación +3.3V |
| GND   | GND   | Tierra |
| MOSI  | GPIO 23 | SPI Master Out Slave In |
| MISO  | GPIO 19 | SPI Master In Slave Out |
| SCK   | GPIO 18 | SPI Clock |
| SS (SDA) | GPIO 5 | Slave Select / Chip Enable |
| RST   | GPIO 21 | Reset |
| IRQ   | (no conectar) | Interrupt (opcional) |

## Conexión del Servomotor

| Servo | ESP32 | Cable |
|-------|-------|-------|
| VCC   | 5V    | Rojo |
| GND   | GND   | Negro/Marrón |
| SIG   | GPIO 2 | Naranja/Amarillo |

**Nota:** El servo requiere más corriente. Se recomienda usar una fuente de alimentación separada de 5V si el ESP32 no proporciona suficiente corriente.

## Conexión del LED de acceso permitido (Verde)

| LED  | ESP32 |
|------|-------|
| (+)  | GPIO 25 |
| (-)  | GND + Resistencia 220Ω |

## Conexión del LED de acceso denegado (Rojo)

| LED  | ESP32 |
|------|-------|
| (+)  | GPIO 26 |
| (-)  | GND + Resistencia 220Ω |

## Conexión del Buzzer

| Buzzer | ESP32 |
|--------|-------|
| (+)    | GPIO 27 |
| (-)    | GND |

## Conexión del Sensor Ultrasónico HC-SR04

| Sensor | ESP32 | Descripción |
|--------|-------|------------|
| VCC    | 5V    | Alimentación +5V |
| GND    | GND   | Tierra |
| TRIG   | GPIO 14 | Trigger (señal de inicio) |
| ECHO   | GPIO 15 | Echo (retorno de la onda) |

**Nota:** El sensor ultrasónico detecta presencia dentro de un rango de 50cm. Si no hay presencia detectada, la puerta se cierra automáticamente.

## Tabla completa de GPIO

| GPIO | Función | Dispositivo |
|------|---------|------------|
| 2    | SIG     | Servomotor |
| 5    | SS      | RC522 |
| 14   | TRIG    | Sensor Ultrasónico |
| 15   | ECHO    | Sensor Ultrasónico |
| 18   | SCK     | RC522 |
| 19   | MISO    | RC522 |
| 21   | RST     | RC522 |
| 23   | MOSI    | RC522 |
| 25   | Output  | LED Verde (Permitido) |
| 26   | Output  | LED Rojo (Denegado) |
| 27   | PWM     | Buzzer |

## Consideraciones de alimentación

- **ESP32:** USB o fuente de 5V + regulador a 3.3V
- **RC522:** 3.3V (bajo consumo, ~100mA máx)
- **Servomotor:** 5V (800mA a 1A, requiere fuente separada)
- **LEDs:** 3.3V con resistencias 220Ω
- **Buzzer:** 3.3V

## Cable SPI típico

```
╔════════════════════════════════════════╗
║         SPI Connections                ║
╠════════════════════════════════════════╣
║ ESP32          RC522    Nombre        ║
║ ───────────────────────────────────── ║
║  GPIO 23 ──────── MOSI  Master Out   ║
║  GPIO 19 ──────── MISO  Master In    ║
║  GPIO 18 ──────── SCK   Clock        ║
║  GPIO 5  ──────── SS    Chip Select  ║
║  GPIO 21 ──────── RST   Reset        ║
║  3V3    ──────── VCC    Power        ║
║  GND    ──────── GND    Ground       ║
╚════════════════════════════════════════╝
```

## Verificación de conexiones

1. Verifica que el RC522 esté correctamente alimentado (LED rojo debe encender, si lo tiene)
2. Verifica que los cables SPI estén bien conectados
3. Carga el firmware y revisa el monitor serial
4. Verifica el mensaje `[RFID] Módulo RC522 inicializado correctamente`
5. Prueba acercando una tarjeta RFID

## Solución de problemas

- **RC522 no detecta tarjetas:** Revisa alimentación y conexión SPI
- **Servo no se mueve:** Verifica alimentación separada de 5V
- **LEDs no encienden:** Verisa polaridad y resistencias
- **Buzzer silencioso:** Verfica que GPIO 27 esté bien conectado
