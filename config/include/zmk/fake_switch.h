#ifndef ZMK_FAKE_SWITCH_H
#define ZMK_FAKE_SWITCH_H

#include <stdint.h>
#include <stdbool.h>

int fake_switch_simulate(uint8_t row, uint8_t col, bool pressed);

#endif // ZMK_FAKE_SWITCH_H
