#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zmk/uart_switch_right.h>

#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_right(row, col, true);   // Pressiona
    k_msleep(50);
    uart_switch_simulate_right(row, col, false);  // Solta
    k_msleep(50);
}

// Pisca o LED de um pino específico
static void blink_pin(uint8_t port_num, uint8_t pin_num, uint32_t delay_ms) {
    const struct device *port = (port_num == 0)
        ? DEVICE_DT_GET(DT_NODELABEL(gpio0))
        : DEVICE_DT_GET(DT_NODELABEL(gpio1));

    if (!device_is_ready(port)) return;

    gpio_pin_configure(port, pin_num, GPIO_OUTPUT);
    gpio_pin_set(port, pin_num, 1);
    k_msleep(delay_ms);
    gpio_pin_set(port, pin_num, 0);
    k_msleep(delay_ms);
}

// Thread de teste
static void test_led_thread(void) {
    // Lista de pinos candidatos (port, pin)
    struct {
        uint8_t port;
        uint8_t pin;
    } test_pins[] = {
        {0, 13}, // A ok
        {0, 15}, // S ok
        {0, 17}, // D
        {0, 25}, // F
        {1, 6},  // G
        {1, 9},  // H
        {0, 24}, // J
        {0, 31}, // K
    };

    while (true) {
        for (int i = 0; i < 2; i++) {
            // Mostra visualmente qual pino está sendo testado
            send_key(1, i + 1);   // A..K (1,1) até (1,8)
            blink_pin(test_pins[i].port, test_pins[i].pin, 300);
            k_msleep(700); // pausa entre testes
        }
    }
}

K_THREAD_DEFINE(test_led_thread_id, 1024, test_led_thread, NULL, NULL, NULL, 5, 0, 0);
