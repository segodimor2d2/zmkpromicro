#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#include "zmk/zmk_mouse_state_changed.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int on_mouse_state_changed(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev) {
        LOG_WRN("Evento inválido");
        return 0;
    }

    LOG_INF("Mouse move dx=%d dy=%d", ev->dx, ev->dy);

    struct zmk_hid_mouse_report *report = zmk_hid_get_mouse_report();
    report->body.d_x = ev->dx;
    report->body.d_y = ev->dy;
    zmk_endpoints_send_mouse_report();

    return 0;
}

ZMK_LISTENER(mouse_state_listener, on_mouse_state_changed);
ZMK_SUBSCRIPTION(mouse_state_listener, zmk_mouse_state_changed);
