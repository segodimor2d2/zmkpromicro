#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_mouse_state_changed {
    int8_t dx;
    int8_t dy;
    int8_t scroll_y;
    int8_t scroll_x;
    uint8_t buttons;
};

// Macro para registrar o evento no ZMK
ZMK_EVENT_DECLARE(zmk_mouse_state_changed);
