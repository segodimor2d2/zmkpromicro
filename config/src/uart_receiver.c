#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

#include <zmk/hid.h>

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

static uint8_t buf[3];
static int buf_pos = 0;

static struct k_sem uart_data_sem;
K_THREAD_STACK_DEFINE(led_stack, 512);
static struct k_thread led_thread_data;

void led_blink_thread(void *a, void *b, void *c)
{
    while (1) {
        k_sem_take(&uart_data_sem, K_FOREVER);
        gpio_pin_set_dt(&led, 1);
        k_sleep(K_MSEC(100));
        gpio_pin_set_dt(&led, 0);
    }
}

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_fifo_read(dev, &c, 1) > 0) {
        buf[buf_pos++] = c;

        if (buf_pos == 3) {
            uint8_t event_type = buf[0];  // 0x01 = press, 0x00 = release
            uint8_t keycode = buf[1];     // keycode de 0x04 a 0x65 etc.
            uint8_t modifiers = buf[2];   // não usado por enquanto

            printk("UART: event=%d keycode=0x%02x mods=0x%02x\n", event_type, keycode, modifiers);

            if (event_type == 0x01) {
                zmk_hid_press(keycode);
                printk("Pressionado\n");
            } else if (event_type == 0x00) {
                zmk_hid_release(keycode);
                printk("Solto\n");
            } else {
                printk("Tipo de evento desconhecido\n");
            }

            k_sem_give(&uart_data_sem);
            buf_pos = 0;
        }
    }
}

void uart_receiver_init(void)
{
    if (!device_is_ready(led.port)) {
        printk("LED device not ready\n");
        return;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    if (!device_is_ready(uart)) {
        printk("UART device not ready\n");
        return;
    }

    k_sem_init(&uart_data_sem, 0, 1);

    uart_irq_callback_user_data_set(uart, uart_cb, NULL);
    uart_irq_rx_enable(uart);

    k_thread_create(&led_thread_data, led_stack, K_THREAD_STACK_SIZEOF(led_stack),
                    led_blink_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);

    printk("UART Receiver iniciado\n");
}

static int uart_receiver_sys_init(void)
{
    uart_receiver_init();
    return 0;
}

SYS_INIT(uart_receiver_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
