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


static int handle_sensor_event(const zmk_event_t *eh) {
    const struct zmk_sensor_event *event = as_zmk_sensor_event(eh);

    if (!event) {
        return 0;
        send_key(2, 2);//x
    }

    send_key(0, 6);//y

    int8_t dx = event->channel_data[0].value.val1;
    int8_t dy = event->channel_data[0].value.val2;

    // Chama sua função existente (definida em uart_move_mouse_left.h)
    uart_move_mouse_left(
        dx,       // deslocamento horizontal
        dy,       // deslocamento vertical
        0,        // scroll_y (não usado por enquanto)
        0,        // scroll_x (não usado)
        0         // buttons (sem cliques)
    );

    return 0;
}

ZMK_LISTENER(mouse_listener, handle_sensor_event);
ZMK_SUBSCRIPTION(mouse_listener, zmk_sensor_event);
