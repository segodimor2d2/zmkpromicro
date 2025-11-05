#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include <zmk/uart_move_mouse_right.h>

#include <zmk/event_manager.h>
// #include <zmk/events/mouse_split_event.h>

#include <zmk/split_mouse_service.h>
// #include <zmk/led_debug.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int uart_move_mouse_right(int8_t dx,
                          int8_t dy,
                          int8_t scroll_y,
                          int8_t scroll_x,
                          uint8_t buttons) {

    LOG_DBG("uart_move_mouse_right: dx=%d dy=%d scroll_x=%d scroll_y=%d buttons=%d",
            dx, dy, scroll_x, scroll_y, buttons);

    uint8_t payload[6] = {
        0x02,
        (uint8_t)dx,
        (uint8_t)dy,
        (uint8_t)scroll_y,
        (uint8_t)scroll_x,
        buttons
    };

    split_mouse_notify(payload, sizeof(payload));

    // Opcional: indicar sucesso com uma tecla fake (para debug visual)
    // led_blink_pattern(1, 60);

    return 0;
}
