#include <zephyr/input/input.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/uart_switch_right.h>

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Pressionar
    k_msleep(20);
    uart_switch_simulate_right(row, col, false);  // Soltar
    k_msleep(20);
}

// Thread de teste para gerar eventos de mouse periodicamente
static void test_mouse_thread(void) {

    while (1) {
        k_sleep(K_SECONDS(2));

        // // Instancia o evento
        // struct zmk_mouse_state_changed ev = {
        //     .dx = 10,
        //     .dy = 0,
        // };
        // // Envia o evento pelo sistema de eventos do ZMK
        // ZMK_EVENT_RAISE(ev);

        // input_report_rel(dev, INPUT_REL_X, 10, false, K_FOREVER);
        // input_report_rel(dev, INPUT_REL_Y, 0, true, K_FOREVER);

        // Teste com envio de tecla
        send_key(0, 6);  // Simula tecla 'Y'
    }
}

// Cria a thread
K_THREAD_DEFINE(test_mouse_id, 1024, test_mouse_thread, NULL, NULL, NULL, 7, 0, 0);
