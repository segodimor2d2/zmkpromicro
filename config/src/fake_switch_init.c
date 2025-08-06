#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zmk/fake_switch.h>

// Thread que vai simular o switch após 2 segundos
void fake_switch_thread(void) {
    k_sleep(K_SECONDS(2));

    fake_switch_simulate(1, 3, true);
    k_sleep(K_MSEC(100));
    fake_switch_simulate(1, 3, false);
}

// Ponteiro para a thread
K_THREAD_DEFINE(fake_switch_tid, 1024, fake_switch_thread, NULL, NULL, NULL, 7, 0, 0);

// Função de init (opcional se quiser rodar código aqui também)
static int fake_switch_init(void) {
    return 0;
}

// Registra a função de init para rodar na inicialização
SYS_INIT(fake_switch_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
