#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/uart_switch.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Defina o número de colunas da sua matriz lógica (Corne = 12 colunas)
#define MATRIX_COLS 12

// Calcula o índice linear a partir de (row, col)
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

int uart_switch_simulate(uint8_t row, uint8_t col, bool pressed) {
    uint8_t layer = 0;

    // Segurança: evita acessar posições inválidas
    // if (row >= 4 || col >= MATRIX_COLS) {
    //     LOG_ERR("Invalid key position: row=%d, col=%d", row, col);
    //     return -EINVAL;
    // }

    uint32_t position = ZMK_KEYMAP_POSITION(row, col);

    const struct zmk_behavior_binding *binding =
        zmk_keymap_get_layer_binding_at_idx(layer, position);

    if (!binding) {
        LOG_ERR("No binding found at (%d, %d)", row, col);
        return -EINVAL;
    }

    struct zmk_behavior_binding_event event = {
        .layer = layer,
        .position = position,
        .timestamp = k_uptime_get(),
    };

    int ret = zmk_behavior_invoke_binding(binding, event, pressed);
    LOG_DBG("uart_switch %s at (%d, %d) => position %d, result: %d",
            pressed ? "press" : "release", row, col, position, ret);
    return ret;
}
