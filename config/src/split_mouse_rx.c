// split_mouse_rx.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/mouse_split_event.h>
#include <zmk/uart_move_mouse_left.h>

LOG_MODULE_REGISTER(split_mouse_rx, CONFIG_ZMK_LOG_LEVEL);

static int handle_split_mouse(const zmk_event_t *eh) {

    const struct zmk_mouse_split_event *ev = as_zmk_mouse_split_event(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    LOG_DBG("Received split mouse dx=%d dy=%d", ev->dx, ev->dy);

    // Chama a função que já existe no lado central para enviar HID mouse report
    (void)uart_move_mouse_left(ev->dx, ev->dy, 0, 0, 0);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(split_mouse_rx_listener, handle_split_mouse);
ZMK_SUBSCRIPTION(split_mouse_rx_listener, zmk_mouse_split_event);
