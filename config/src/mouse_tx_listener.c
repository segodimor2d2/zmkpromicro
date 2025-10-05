#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include "zmk_mouse_state_changed.h"
#include <zmk/split/bluetooth/service.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);


static int mouse_tx_listener(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev)
        return ZMK_EV_EVENT_BUBBLE;

#if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_PERIPHERAL)
    int ret = zmk_split_bt_peripheral_send((uint8_t *)ev, sizeof(*ev));
    if (ret == 0) {
        LOG_INF("Enviado via BLE Split -> dx=%d dy=%d scroll_x=%d scroll_y=%d btn=%d",
                ev->dx, ev->dy, ev->scroll_x, ev->scroll_y, ev->buttons);
    } else {
        LOG_ERR("Falha ao enviar evento via split BLE (ret=%d)", ret);
    }
#endif
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(mouse_tx_listener, mouse_tx_listener);
ZMK_SUBSCRIPTION(mouse_tx_listener, zmk_mouse_state_changed);
