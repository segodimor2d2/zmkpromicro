#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>
#include <zmk/uart_switch_right.h> // seu código de envio de keycodes
#include <zmk/zmk_mouse_state_changed.h> 

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Press
    k_msleep(20); // Pequena pausa para o host reconhecer a tecla
    uart_switch_simulate_right(row, col, false);  // Release
    k_msleep(20);
}

// Função para enviar a palavra "oi"
static void send_test_message() {
    // Ajuste os valores de (row, col) de acordo com seu keymap
    send_key(0, 6); // 'u'
    // send_key(0, 8); // 'i'
}

// Listener que reage a um evento qualquer (ex: zmk_mouse_state_changed) test_listener
static int test_listener_cb(const zmk_event_t *eh) {
    // Aqui você pode escolher qual evento vai disparar a mensagem
    // Por exemplo, qualquer evento de mouse ou teclado
    send_test_message();
    return 0;
}

// Registra listener e subscription
ZMK_LISTENER(test_right, test_listener_cb);
ZMK_SUBSCRIPTION(test_right, zmk_mouse_state_changed); // ou outro evento que queira usar
