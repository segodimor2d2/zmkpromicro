#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zmk/uart_move_mouse_right.h>
#include <zmk/uart_switch_right.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

const struct device *dev;

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Press
    k_msleep(20);
    uart_switch_simulate_right(row, col, false);  // Release
    k_msleep(20);
}

// Recebe dados UART e envia movimento relativo.
int uart_move_mouse_right(int8_t dx,
                          int8_t dy,
                          int8_t scroll_y,
                          int8_t scroll_x,
                          uint8_t buttons) {

  
    int ret;

    ret = input_report_rel(dev, INPUT_REL_X, dx, false, K_FOREVER);
    if (ret == 0) {
        send_key(1, 1); // a → evento válido
    } else {
        send_key(2, 2); // x → evento válido
    }
    ret = input_report_rel(dev, INPUT_REL_Y, dy, true, K_FOREVER);
    if (ret == 0) {
        send_key(1, 2); // s → evento válido
    } else {
        send_key(2, 2); // x → evento válido
    }



    return 0;
}
