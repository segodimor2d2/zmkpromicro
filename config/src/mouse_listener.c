#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/sensor_event.h>
#include <zmk/hid.h>
#include <zmk/uart_switch_left.h>
#include <zmk/uart_move_mouse_left.h>

// #error "!!!!VERIFICANDO SE ESTÁ SENDO COMPILADO!!!!"

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_left(row, col, true);   // Pressionar
    k_msleep(20);
    uart_switch_simulate_left(row, col, false);  // Soltar
    k_msleep(20);
}


static inline bool is_mouse_packet(uint32_t pos) {
    return (pos & (1u << 31)) != 0;
}

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

    // ignore events que não são pacotes de mouse
    if (!is_mouse_packet(p->position)) {
        return ZMK_EV_EVENT_BUBBLE; // permite que keypresses sigam normalmente
    }

    int8_t dx = 0, dy = 0;
    decode_dxdy(p->position, &dx, &dy);

    LOG_DBG("Received split mouse dx=%d dy=%d", dx, dy);

    (void)uart_move_mouse_left(dx, dy, 0, 0, 0);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(mouse_listener, handle_sensor_event);
ZMK_SUBSCRIPTION(mouse_listener, zmk_sensor_event);
