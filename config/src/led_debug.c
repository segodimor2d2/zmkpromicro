#include <zmk/led_debug.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_debug, CONFIG_ZMK_LOG_LEVEL);

// LED interno no P0.13 ou P0.15
#define LED_PORT   DT_NODELABEL(gpio0)
#define LED_PIN    15  // altere para 15 se quiser outro LED

static const struct device *port;

void led_debug_init(void) {
    port = DEVICE_DT_GET(LED_PORT);

    if (!device_is_ready(port)) {
        LOG_ERR("GPIO port not ready");
        return;
    }

    gpio_pin_configure(port, LED_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_pin_set(port, LED_PIN, 0); // começa desligado

    LOG_INF("LED P0.%d initialized", LED_PIN);
}

void led_blink_pattern(uint8_t count, uint32_t delay_ms) {
    if (!port || !device_is_ready(port)) {
        LOG_ERR("LED port not initialized or not ready");
        return;
    }

    for (int i = 0; i < count; i++) {
        gpio_pin_set(port, LED_PIN, 1);
        k_msleep(delay_ms);
        gpio_pin_set(port, LED_PIN, 0);
        k_msleep(delay_ms);
    }
}

void led_set(bool state) {
    if (!port || !device_is_ready(port)) {
        LOG_ERR("LED port not initialized or not ready");
        return;
    }

    gpio_pin_set(port, LED_PIN, state ? 1 : 0);
}

// ✅ Inicialização automática no boot
static int led_debug_init_wrapper(const struct device *unused)
{
    ARG_UNUSED(unused);
    led_debug_init();
    return 0;
}

SYS_INIT(led_debug_init_wrapper, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
