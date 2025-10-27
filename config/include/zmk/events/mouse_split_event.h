#pragma once

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>

struct zmk_mouse_split_event {
    zmk_event_t header;  // Campo obrigatório para todos os eventos do ZMK
    int8_t dx;
    int8_t dy;
    int8_t scroll_x;
    int8_t scroll_y;
    uint8_t buttons;
};

ZMK_EVENT_DECLARE(zmk_mouse_split_event);
