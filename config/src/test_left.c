#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include "zmk_mouse_state_changed.h"
#include <zmk/uart_switch_left.h>

#define MATRIX_COLS 12

static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_left(row, col, true);
    k_msleep(20);
    uart_switch_simulate_left(row, col, false);
    k_msleep(20);
}

static int central_mouse_listener(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    send_key(0, 0); // Indicação que entrou no listener
    if (!ev) {
        send_key(2, 2); // X → evento nulo
        return ZMK_EV_EVENT_BUBBLE;
    }

    send_key(1, 1); // L → evento válido
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(central_mouse_listener, central_mouse_listener);
ZMK_SUBSCRIPTION(central_mouse_listener, zmk_mouse_state_changed);

// --- Força um evento de teste no boot ---
static int test_startup(void) {
    struct zmk_mouse_state_changed test_ev = {
        .dx = 1,
        .dy = 2,
        .scroll_x = 0,
        .scroll_y = 0,
        .buttons = 0,
    };

    // Levanta o evento manualmente
    ZMK_EVENT_RAISE(test_ev);

    return 0;
}

SYS_INIT(test_startup, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
