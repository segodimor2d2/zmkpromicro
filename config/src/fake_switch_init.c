#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zmk/uart_switch.h>

// Thread que vai simular o switch após 2 segundos
void uart_switch_thread(void) {
    k_sleep(K_SECONDS(2));

    uart_switch_simulate(0, 1, true);
    k_sleep(K_MSEC(100));
    uart_switch_simulate(0, 1, false);

    uart_switch_simulate(1, 1, true);
    k_sleep(K_MSEC(100));
    uart_switch_simulate(1, 1, false);

    uart_switch_simulate(2, 1, true);
    k_sleep(K_MSEC(100));
    uart_switch_simulate(2, 1, false);

}

// Ponteiro para a thread
K_THREAD_DEFINE(uart_switch_tid, 1024, uart_switch_thread, NULL, NULL, NULL, 7, 0, 0);

// Função de init (opcional se quiser rodar código aqui também)
static int uart_switch_init(void) {
    return 0;
}

// Registra a função de init para rodar na inicialização
SYS_INIT(uart_switch_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
