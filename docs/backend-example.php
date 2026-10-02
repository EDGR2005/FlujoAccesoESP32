<?php
header('Content-Type: application/json');

try {
    $raw = file_get_contents('php://input');
    $data = json_decode($raw, true);

    if (!$data || !isset($data['uid'])) {
        http_response_code(400);
        echo json_encode([
            'granted' => false,
            'is_admin' => false,
            'message' => 'UID no recibido'
        ]);
        exit;
    }

    $uid = strtoupper(trim((string)$data['uid']));

    // Ejemplo de validación de UID
    $allowed = ['A1B2C3D4', '01A2B3C4'];
    $adminUids = ['A1B2C3D4'];

    $granted = in_array($uid, $allowed, true);
    $isAdmin = in_array($uid, $adminUids, true);

    echo json_encode([
        'granted' => $granted,
        'is_admin' => $isAdmin,
        'message' => $granted ? 'Acceso concedido' : 'Acceso denegado',
        'redirect_url' => $isAdmin ? 'http://192.168.1.98/admin' : null
    ]);
} catch (Exception $e) {
    http_response_code(500);
    echo json_encode([
        'granted' => false,
        'is_admin' => false,
        'message' => 'Error en el backend'
    ]);
}
