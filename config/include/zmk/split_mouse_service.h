#pragma once

#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Envia um payload de mouse via BLE para o central.
 * Tamanho recomendado: 6 bytes.
 */
int split_mouse_notify(const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
