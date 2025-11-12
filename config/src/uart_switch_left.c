#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/uart_switch_left.h>
#include <zmk/events/position_state_changed.h>  // necessário para raise_zmk_position_state_changed

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// #error "!!!!VERIFICANDO SE ESTÁ SENDO COMPILADO!!!!"

// Número de colunas da matriz lógica (Corne = 12 colunas)
#define MATRIX_COLS 12

// Calcula índice linear a partir de (row, col)
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

int uart_switch_simulate_left(uint8_t row, uint8_t col, bool pressed) {
    uint32_t position = ZMK_KEYMAP_POSITION(row, col);

    struct zmk_position_state_changed event = {
        .source = ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL,
        .state = pressed,
        .position = position,
        .timestamp = k_uptime_get(),
    };

    int ret = raise_zmk_position_state_changed(event);
    LOG_DBG("uart_switch LEFT %s at (row=%d, col=%d) => position %d, result: %d",
            pressed ? "press" : "release", row, col, position, ret);

    return ret;
}
