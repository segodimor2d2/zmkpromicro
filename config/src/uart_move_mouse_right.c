#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zmk/uart_move_mouse_right.h>
#include <zmk/uart_switch_right.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);


// const struct device *dev;
// static const struct device *dev = DEVICE_DT_GET_ANY(zmk_input_device);
// static const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zmk_input_device));

// Obter o dispositivo do mouse input
// static const struct device *mouse_dev = DEVICE_DT_GET(DT_NODELABEL(zmk_input_mouse));

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



    // input_report_rel(dev, INPUT_REL_X, dx, false, K_FOREVER);
    // input_report_rel(dev, INPUT_REL_Y, dy, true, K_FOREVER);
    // send_key(1, 1); // a → evento válido
  
    // int ret_x = input_report_rel(dev, INPUT_REL_X, dx, false, K_FOREVER);
    // int ret_y = input_report_rel(dev, INPUT_REL_Y, dy, true, K_FOREVER);
    //
    // if (ret_x == 0 && ret_y == 0) {
    // //
        // struct zmk_mouse_state_changed ev = {
        //     .dx = dx,
        //     .dy = dy
        //     // .scroll_y = scroll_y,
        //     // .scroll_x = scroll_x,
        //     // .buttons = buttons,
        // };
        //
        // ZMK_EVENT_RAISE(ev);
        // send_key(1, 1); // A → sucesso
    //
    // } else {
    //     send_key(2, 2); // X → erro
    // }


    // LOG_INF(ret);

    send_key(1, 1); // A → sucesso


    return 0;
}
