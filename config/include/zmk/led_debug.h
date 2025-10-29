#pragma once
#include <stdbool.h>
#include <zephyr/kernel.h>

// Inicializa o LED
void led_debug_init(void);

// Pisca o LED um número de vezes
void led_blink_pattern(uint8_t count, uint32_t delay_ms);

// Liga/desliga o LED diretamente
void led_set(bool state);
