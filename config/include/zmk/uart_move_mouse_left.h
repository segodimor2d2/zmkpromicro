#pragma once
#include <zephyr/kernel.h>
#include <zmk/hid.h>

// Agora todos os deslocamentos são int8_t
int uart_move_mouse(int8_t dx, int8_t dy,
                    int8_t scroll_y, int8_t scroll_x,
                    zmk_mouse_button_flags_t buttons);
