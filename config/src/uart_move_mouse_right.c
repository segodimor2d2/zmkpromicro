#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include <zmk/uart_move_mouse_right.h>

#include <zmk/event_manager.h>
#include <zmk/events/mouse_split_event.h>

#include <zmk/led_debug.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int uart_move_mouse_right(int8_t dx,
                          int8_t dy,
                          int8_t scroll_y,
                          int8_t scroll_x,
                          uint8_t buttons) {

    led_debug_init();

    LOG_DBG("uart_move_mouse_right: dx=%d dy=%d scroll_x=%d scroll_y=%d buttons=%d",
            dx, dy, scroll_x, scroll_y, buttons);


    // Prepara o evento customizado
    struct zmk_mouse_split_event ev = {
        .dx = dx,
        .dy = dy,
        .scroll_x = scroll_x,
        .scroll_y = scroll_y,
        .buttons = buttons,
    };

    // Envia o evento pelo Event Manager (será transportado via split)
    ZMK_EVENT_RAISE(ev);

    // Opcional: indicar sucesso com uma tecla fake (para debug visual)
    led_blink_pattern(1, 200);

    return 0;
}
