#pragma once

#include <zephyr/kernel.h>
#include <zmk/hid.h>

int uart_move_mouse_left(
    int8_t dx,
    int8_t dy,
    int8_t scroll_y,
    int8_t scroll_x,
    uint8_t buttons
);


