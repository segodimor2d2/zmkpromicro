#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/keymap.h>
#include <zmk/behavior.h>

#include <zmk/fake_switch.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Use com cautela: verifique se (row << 8 | col) corresponde ao índice da matriz keymap
#define ZMK_KEYMAP_POSITION(row, col) (((uint32_t)(row) << 8) | (col))

int fake_switch_simulate(uint8_t row, uint8_t col, bool pressed) {
    uint8_t layer = 0;
    uint32_t position = ZMK_KEYMAP_POSITION(row, col);

    const struct zmk_behavior_binding *binding = zmk_keymap_get_layer_binding_at_idx(layer, position);

    if (!binding) {
        LOG_ERR("No binding at (%d,%d)", row, col);
        return -EINVAL;
    }

    struct zmk_behavior_binding_event event = {
        .layer = layer,
        .position = position,
        .timestamp = k_uptime_get(),
    };

    int ret = zmk_behavior_invoke_binding(binding, event, pressed);
    LOG_DBG("fake_switch %s at (%d,%d): %d", pressed ? "press" : "release", row, col, ret);
    return ret;
}
