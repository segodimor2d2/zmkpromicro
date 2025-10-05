#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include "zmk_mouse_state_changed.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Função da thread que dispara eventos de teste do mouse
void mouse_test_thread(void) {
    for (int i = 1; i <= 5; i++) {
        struct zmk_mouse_state_changed ev = {
            .dx = i,
            .dy = i,
            .scroll_x = 0,
            .scroll_y = 0,
            .buttons = 0,
        };

        // Inicializa o header com o tipo correto do evento
        ev.header.event = &zmk_event_zmk_mouse_state_changed;

        // Dispara o evento para o Event Manager do ZMK
        ZMK_EVENT_RAISE(ev);

        LOG_INF("Mouse event raised: dx=%d, dy=%d", ev.dx, ev.dy);

        k_msleep(1000); // espera 1 segundo entre os eventos
    }
}

// Define a thread do Zephyr
K_THREAD_DEFINE(mouse_test_thread_id, 1024, mouse_test_thread, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);
