#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include "zmk_mouse_state_changed.h"
#include <zmk/uart_move_mouse_left.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int zmk_split_bt_central_receive(const uint8_t *data, size_t len)
{
    if (len != sizeof(struct zmk_mouse_state_changed))
        return -EINVAL;

    const struct zmk_mouse_state_changed *recv = (const struct zmk_mouse_state_changed *)data;

    struct zmk_mouse_state_changed ev = {
        .dx = recv->dx,
        .dy = recv->dy,
        .scroll_x = recv->scroll_x,
        .scroll_y = recv->scroll_y,
        .buttons = recv->buttons,
    };
    ev.header.event = &zmk_event_zmk_mouse_state_changed;

    ZMK_EVENT_RAISE(ev);

    LOG_INF("Mouse event via BLE Split: dx=%d dy=%d scroll_y=%d scroll_x=%d buttons=0x%02X",
            ev.dx, ev.dy, ev.scroll_y, ev.scroll_x, ev.buttons);

    uart_move_mouse(ev.dx, ev.dy, ev.scroll_y, ev.scroll_x, ev.buttons);

    return 0;
}
