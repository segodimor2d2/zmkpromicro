#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/mouse_split_event.h>

LOG_MODULE_REGISTER(split_mouse_rx, CONFIG_ZMK_LOG_LEVEL);

int uart_move_mouse_left(
    int8_t dx,
    int8_t dy,
    int8_t scroll_y,
    int8_t scroll_x,
    uint8_t buttons
);

static int handle_split_mouse(const zmk_event_t *eh) {
    const struct zmk_mouse_split_event *ev = as_zmk_mouse_split_event(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    LOG_DBG("Received zmk_mouse_split_event dx=%d dy=%d scroll_x=%d scroll_y=%d buttons=%d",
            ev->dx, ev->dy, ev->scroll_x, ev->scroll_y, ev->buttons);

    uart_move_mouse_left(ev->dx, ev->dy, ev->scroll_y, ev->scroll_x, ev->buttons);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(split_mouse_rx_listener, handle_split_mouse);
ZMK_SUBSCRIPTION(split_mouse_rx_listener, zmk_mouse_split_event);
