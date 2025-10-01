/* uart_move_mouse_right.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <zmk/uart_move_mouse_right.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define EVT_MOUSE 0x02
static const struct device *uart_right = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Envia movimento de mouse do peripheral para o central */
int uart_move_mouse_right(int8_t dx, int8_t dy, int8_t scroll_x, int8_t scroll_y, uint8_t buttons) {
    if (!device_is_ready(uart_right)) {
        LOG_ERR("UART device not ready");
        return -ENODEV;
    }

    uint8_t buf[7];
    buf[0] = 0xAA;       // header
    buf[1] = EVT_MOUSE;  // tipo do evento
    buf[2] = (uint8_t)dx;
    buf[3] = (uint8_t)dy;
    buf[4] = (uint8_t)scroll_x;
    buf[5] = (uint8_t)scroll_y;
    buf[6] = buttons;

    /* checksum simples (XOR dos bytes 1..6) */
    uint8_t checksum = 0;
    for (int i = 1; i <= 6; i++) {
        checksum ^= buf[i];
    }

    /* envia pacote */
    for (int i = 0; i < sizeof(buf); i++) {
        uart_poll_out(uart_right, buf[i]);
    }
    uart_poll_out(uart_right, checksum); // envia checksum no final

    LOG_DBG("Mouse RIGHT → LEFT enviado: dx=%d dy=%d scrollX=%d scrollY=%d btn=%02x",
            dx, dy, scroll_x, scroll_y, buttons);

    return 0;
}
