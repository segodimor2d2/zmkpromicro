#include <zmk/event_manager.h>
#include "zmk_mouse_state_changed.h"
#include <zmk/uart_move_mouse_left.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int mouse_rx_listener(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev)
        return ZMK_EV_EVENT_BUBBLE;

    // Envia os eventos recebidos para o UART
    uart_move_mouse(ev->dx, ev->dy, ev->scroll_y, ev->scroll_x, ev->buttons);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(mouse_rx_listener, mouse_rx_listener);
ZMK_SUBSCRIPTION(mouse_rx_listener, zmk_mouse_state_changed);
