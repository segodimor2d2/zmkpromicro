#pragma once

#include <zephyr/kernel.h>

struct my_mouse_report {
    int8_t dx;
    int8_t dy;
    int8_t scroll_x;
    int8_t scroll_y;
    uint32_t buttons;
} __packed;
