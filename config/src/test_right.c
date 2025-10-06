#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/uart_switch_right.h>
#include "zmk_mouse_state_changed.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Press
    k_msleep(20);
    uart_switch_simulate_right(row, col, false);  // Release
    k_msleep(20);
}

static int mouse_tx_listener(const zmk_event_t *eh) {
    const struct zmk_mouse_state_changed *ev = as_zmk_mouse_state_changed(eh);
    if (!ev) {
        send_key(2, 2); // X ou outra indicação visual
        return ZMK_EV_EVENT_BUBBLE;
    }

    // Cria um novo evento com os mesmos valores
    struct zmk_mouse_state_changed new_ev = {
        .dx = ev->dx,
        .dy = ev->dy,
        .scroll_x = ev->scroll_x,
        .scroll_y = ev->scroll_y,
        .buttons = ev->buttons,
    };

    // Levanta o evento: o ZMK cuida da transmissão para o central
    ZMK_EVENT_RAISE(new_ev);

    // Indicação visual para teste
    send_key(0, 4); // 'r'
    LOG_INF("Evento levantado e enviado via split -> dx=%d dy=%d scroll_x=%d scroll_y=%d btn=%d",
            ev->dx, ev->dy, ev->scroll_x, ev->scroll_y, ev->buttons);

    return ZMK_EV_EVENT_BUBBLE; // <-- Corrigido
}

ZMK_LISTENER(mouse_tx_listener, mouse_tx_listener);
ZMK_SUBSCRIPTION(mouse_tx_listener, zmk_mouse_state_changed);
