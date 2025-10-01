#pragma once
#include <stdint.h>
#include <zephyr/drivers/uart.h>

extern const struct device *uart_dev; // definido em uart_receiver_right.c

// Agora com int8_t
int uart_move_mouse_right(int8_t dx, int8_t dy,
    int8_t scroll_x, int8_t scroll_y, uint8_t buttons);
