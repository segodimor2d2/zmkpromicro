/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/settings/settings.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY)
#include <zmk/display.h>
#include <lvgl.h>
#endif

// Declaração da função UART implementada em uart_receiver.c
extern int uart_receiver_init(void);

// Adiciona esse bloco antes de `main()`
#ifndef CONFIG_UART_CONSOLE
#warning "UART console is not enabled. uart_receiver_init() may fail."
#endif

int main(void) {
    LOG_INF("Welcome to ZMK!\n");

#if IS_ENABLED(CONFIG_SETTINGS)
    settings_subsys_init();
    settings_load();
#endif

    int err = uart_receiver_init();
    if (err) {
        LOG_ERR("UART receiver initialization failed");
    }

#ifdef CONFIG_ZMK_DISPLAY
    zmk_display_init();

#if IS_ENABLED(CONFIG_ARCH_POSIX)
    // Workaround for SDL display issue
    while (1) {
        lv_task_handler();
        k_sleep(K_MSEC(10));
    }
#endif

#endif /* CONFIG_ZMK_DISPLAY */

    return 0;
}
