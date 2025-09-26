#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#include <zmk/uart_move_mouse.h>

/**
 * Envia um movimento ou clique de mouse via HID para o central.
 *
 * @param dx Deslocamento no eixo X (-127 a 127).
 * @param dy Deslocamento no eixo Y (-127 a 127).
 * @param buttons Máscara de botões (bitfield, ex: 1 = left, 2 = right, 4 = middle).
 *
 * @return 0 em sucesso, ou código de erro.
 */

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

int uart_mouse_move(int8_t dx, int8_t dy, uint8_t buttons) {
    struct zmk_hid_mouse_report_body report = {
        .buttons = buttons,
        .d_x = dx,
        .d_y = dy
    };

    int ret = zmk_endpoints_send_mouse_report(&report);
    LOG_DBG("UART mouse move dx=%d dy=%d buttons=0x%02X ret=%d",
            dx, dy, buttons, ret);

    return ret;
}
