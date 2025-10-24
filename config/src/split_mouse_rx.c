// split_mouse_rx.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

LOG_MODULE_REGISTER(split_mouse_rx, CONFIG_ZMK_LOG_LEVEL);

// Prototipo — seu projeto já tem essa função no lado central.
int uart_move_mouse_left(
    int8_t dx,
    int8_t dy,
    int8_t scroll_y,
    int8_t scroll_x,
    uint8_t buttons
);

// Decode the position back into dx, dy
static inline void decode_dxdy(uint32_t pos, int8_t *dx, int8_t *dy) {
    uint8_t udx = (pos >> 16) & 0xFF;
    uint8_t udy = pos & 0xFF;
    *dx = (int8_t)udx;
    *dy = (int8_t)udy;
}

static int handle_split_mouse(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *p = as_zmk_position_state_changed(eh);
    if (!p) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    int8_t dx = 0, dy = 0;
    decode_dxdy(p->position, &dx, &dy);

    LOG_DBG("Received split mouse dx=%d dy=%d", dx, dy);

    // Chama a função que já existe no lado central para enviar HID mouse report
    // Ajuste scroll/buttons se necessário; aqui só dx/dy.
    (void)uart_move_mouse_left(dx, dy, 0, 0, 0);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(split_mouse_rx_listener, handle_split_mouse);
ZMK_SUBSCRIPTION(split_mouse_rx_listener, zmk_position_state_changed);
