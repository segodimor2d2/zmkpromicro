#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include "zmk/events/zmk_mouse_state_changed.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Função callback do listener
static int mouse_tx_listener_cb(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev)
        return ZMK_EV_EVENT_BUBBLE;

    // Apenas logar o evento de mouse
    LOG_INF("Evento de mouse -> dx=%d dy=%d scroll_x=%d scroll_y=%d btn=%d",
            ev->dx, ev->dy, ev->scroll_x, ev->scroll_y, ev->buttons);

    return ZMK_EV_EVENT_BUBBLE;
}

// Registrar listener
ZMK_LISTENER(mouse_tx_listener, mouse_tx_listener_cb);
// Assinar eventos
ZMK_SUBSCRIPTION(mouse_tx_listener, zmk_mouse_state_changed);
