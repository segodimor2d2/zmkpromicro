#ifndef ZMK_UART_SWITCH_H
#define ZMK_UART_SWITCH_H

#include <stdint.h>
#include <stdbool.h>

int uart_switch_simulate_right(uint8_t row, uint8_t col, bool pressed);

#endif
