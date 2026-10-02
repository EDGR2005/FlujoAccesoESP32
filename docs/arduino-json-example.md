# JSON de ejemplo para el ESP32

## Solicitud enviada por el ESP32

```json
{
  "uid": "A1B2C3D4"
}
```

## Respuesta esperada del backend

### Acceso permitido

```json
{
  "granted": true,
  "is_admin": false,
  "message": "Acceso concedido"
}
```

### Acceso denegado

```json
{
  "granted": false,
  "is_admin": false,
  "message": "Acceso denegado"
}
```

### Acceso de administrador

```json
{
  "granted": true,
  "is_admin": true,
  "message": "Acceso concedido",
  "redirect_url": "http://192.168.1.98/admin"
}
```

## Uso en Arduino

```cpp
StaticJsonDocument<256> doc;

deserializeJson(doc, response);

bool granted = doc["granted"] | false;
bool isAdmin = doc["is_admin"] | false;
const char* message = doc["message"] | "Acceso denegado";
```
