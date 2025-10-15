#pragma once

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_mouse_state_changed {
    zmk_event_t header;  // CORRETO: typedef do event manager
    int8_t dx;
    int8_t dy;
    // int8_t scroll_x;
    // int8_t scroll_y;
    // uint8_t buttons;
};

ZMK_EVENT_DECLARE(zmk_mouse_state_changed);


