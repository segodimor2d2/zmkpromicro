// split_mouse_tx.c
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>

LOG_MODULE_REGISTER(split_mouse_tx, CONFIG_ZMK_LOG_LEVEL);

// Encode dx,dy into a single uint32_t position
static inline uint32_t encode_dxdy(int8_t dx, int8_t dy) {
    uint32_t udx = (uint8_t)dx;
    uint32_t udy = (uint8_t)dy;
    return (udx << 16) | udy;
}

// Public function to call when you want to send mouse motion
void split_mouse_send(int8_t dx, int8_t dy) {
    struct zmk_position_state_changed ev = {
        .source = ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL,
        .state = true,
        .position = encode_dxdy(dx, dy),
        .timestamp = k_uptime_get(),
    };

    // raise the event — position_state_changed is forwarded by split transport
    raise_zmk_position_state_changed(ev);
}

// Optional: small test thread that sends motion periodically
#if 1
static void test_thread(void *) {
    while (1) {
        split_mouse_send(5, -3);
        k_msleep(1000);
    }
}
K_THREAD_DEFINE(split_mouse_tx_thread_id, 1024, test_thread, NULL, NULL, NULL, 5, 0, 0);
#endif
