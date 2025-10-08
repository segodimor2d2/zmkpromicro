#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/zmk_mouse_state_changed.h>
#include <zmk/events/sensor_event.h>
#include <zmk/sensors.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static int mouse_tx_listener(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev)
        return ZMK_EV_EVENT_BUBBLE;

    // Cria um canal de dados de sensor genérico com dx/dy
    struct zmk_sensor_channel_data channel_data = {
        .value = { ev->dx, ev->dy },
    };

    // Cria o evento de sensor
    struct zmk_sensor_event sensor_ev = {
        .channel_data_size = sizeof(struct zmk_sensor_channel_data),
        .sensor_index = 0,
    };

    // Copia os dados do canal para o evento
    memcpy(&sensor_ev.channel_data[0], &channel_data, sizeof(channel_data));

    // Publica o evento (será enviado automaticamente via split)
    ZMK_EVENT_RAISE(sensor_ev);

    LOG_INF("Mouse TX: dx=%d dy=%d scroll_x=%d scroll_y=%d btn=%d",
            ev->dx, ev->dy, ev->scroll_x, ev->scroll_y, ev->buttons);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(mouse_tx_listener, mouse_tx_listener);
ZMK_SUBSCRIPTION(mouse_tx_listener, zmk_mouse_state_changed);
