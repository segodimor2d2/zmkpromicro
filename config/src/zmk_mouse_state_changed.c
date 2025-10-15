#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include "zmk/zmk_mouse_state_changed.h"

void test_mouse_thread(void) {
    while (1) {
        k_sleep(K_SECONDS(2));

        struct zmk_mouse_state_changed ev = {
            .dx = 10,
            .dy = 0,
        };
        ZMK_EVENT_RAISE(ev);
    }
}


// Implementa o evento para o linker
ZMK_EVENT_IMPL(zmk_mouse_state_changed);
