#pragma once
#include <zephyr/kernel.h>

int uart_mouse_move(int8_t dx, int8_t dy, uint8_t buttons);
