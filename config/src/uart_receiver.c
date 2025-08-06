#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zmk/uart_switch.h>

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

// Agora 4 bytes: start_byte + event_type + row + col
static uint8_t buf[4];
static int buf_pos = 0;

static struct k_sem uart_data_sem;
K_THREAD_STACK_DEFINE(led_stack, 512);
static struct k_thread led_thread_data;

// Thread que pisca LED quando um pacote válido é recebido
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
        // Espera sempre pelo start byte 0xAA
        if (buf_pos == 0 && c != 0xAA) {
            continue;
        }

        buf[buf_pos++] = c;

        if (buf_pos == 4) {
            uint8_t event_type = buf[1];  // 0x01 = press, 0x00 = release
            uint8_t row = buf[2];         // linha
            uint8_t col = buf[3];         // coluna

            bool pressed = event_type == 0x01;

            printk("UART: %s (%d,%d)\n", pressed ? "Press" : "Release", row, col);
            printk("Pacote UART recebido: 0x%02X 0x%02X 0x%02X 0x%02X\n",
                   buf[0], buf[1], buf[2], buf[3]);

            // Executa o switch uart
            int ret = uart_switch_simulate(row, col, pressed);
            if (ret < 0) {
                printk("Erro ao simular tecla (%d,%d)\n", row, col);
            }

            k_sem_give(&uart_data_sem);
            buf_pos = 0; // Reinicia buffer
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
