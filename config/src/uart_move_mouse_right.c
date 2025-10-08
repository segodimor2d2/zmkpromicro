#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/uart_move_mouse_right.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int uart_move_mouse_right(int8_t dx,
                         int8_t dy,
                         int8_t scroll_y,
                         int8_t scroll_x,
                         uint8_t buttons) {

    // Apenas imprime os dados recebidos
    LOG_INF("Mouse UART recebido: dx=%d, dy=%d, scroll_y=%d, scroll_x=%d, buttons=0x%02x",
            dx, dy, scroll_y, scroll_x, buttons);

    return 0;
}
