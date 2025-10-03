#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/zmk_mouse_state_changed.h>
#include <zmk/endpoints.h>
#include <zmk/hid.h>
#include <zmk/uart_move_mouse_left.h> // ou uart_move_mouse_right.h conforme o lado

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Callback para tratar eventos de mouse
static int mouse_state_listener_cb(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);

    // if (!ev) {
    //     return 0; // evento não era do tipo esperado
    // }

    LOG_INF("Mouse event: dx=%d, dy=%d, scroll_y=%d, scroll_x=%d, buttons=0x%02X",
            ev->dx, ev->dy, ev->scroll_y, ev->scroll_x, ev->buttons);

    // Pega o report global do ZMK
    struct zmk_hid_mouse_report *report = zmk_hid_get_mouse_report();

    report->body.d_x        = ev->dx;
    report->body.d_y        = ev->dy;
    report->body.d_scroll_y = ev->scroll_y;
    report->body.d_scroll_x = ev->scroll_x;
    report->body.buttons    = ev->buttons;

    int ret = zmk_endpoints_send_mouse_report();

    return ret;
}

ZMK_LISTENER(mouse_state_listener, mouse_state_listener_cb);
ZMK_SUBSCRIPTION(mouse_state_listener, zmk_mouse_state_changed);
