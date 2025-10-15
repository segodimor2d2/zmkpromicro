#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include "zmk/zmk_mouse_state_changed.h"
#include <zmk/uart_switch_right.h>

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Press
    k_msleep(20);
    uart_switch_simulate_right(row, col, false);  // Release
    k_msleep(20);
}

// Thread que gera eventos
static void test_mouse_thread(void) {
    while (5) {
        k_sleep(K_SECONDS(2));

        struct zmk_mouse_state_changed ev = {
            .dx = 10,
            .dy = 0,
        };

        ZMK_EVENT_RAISE(ev);
        send_key(0, 6); // Y → sucesso
    }
}

// Cria o thread
K_THREAD_DEFINE(test_mouse_id, 1024, test_mouse_thread, NULL, NULL, NULL, 7, 0, 0);

// Subscrever eventos — se você quiser reagir a este evento, defina um listener separado
// ZMK_SUBSCRIPTION(test_mouse, zmk_mouse_state_changed);
