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

// Pacote UART: [0xAA][event_type][row][col]
static uint8_t buf[4];
static int buf_pos = 0;

// Estrutura para armazenar evento UART
struct uart_event_t {
    uint8_t event_type;
    uint8_t row;
    uint8_t col;
};

// Fila para armazenar até 10 eventos UART
#define UART_EVENT_QUEUE_SIZE 10
K_MSGQ_DEFINE(uart_event_msgq, sizeof(struct uart_event_t), UART_EVENT_QUEUE_SIZE, 4);

// Stack e thread para processar eventos UART e piscar o LED
K_THREAD_STACK_DEFINE(led_stack, 512);
static struct k_thread led_thread_data;

void led_blink_thread(void *a, void *b, void *c)
{
    struct uart_event_t event;

    while (1) {
        // Espera até que um evento esteja disponível
        k_msgq_get(&uart_event_msgq, &event, K_FOREVER);

        bool pressed = event.event_type == 0x01;

        printk("UART: %s (%d,%d)\n", pressed ? "Press" : "Release", event.row, event.col);
        printk("Pacote UART recebido: 0xAA 0x%02X 0x%02X 0x%02X\n",
               event.event_type, event.row, event.col);

        int ret = uart_switch_simulate(event.row, event.col, pressed);
        if (ret < 0) {
            printk("Erro ao simular tecla (%d,%d)\n", event.row, event.col);
        }

        // Pisca LED como indicação do evento
        gpio_pin_set_dt(&led, 1);
        k_sleep(K_MSEC(100));
        gpio_pin_set_dt(&led, 0);
    }
}

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_fifo_read(dev, &c, 1) > 0) {
        // Aguarda byte inicial 0xAA
        if (buf_pos == 0 && c != 0xAA) {
            continue;
        }

        buf[buf_pos++] = c;

        if (buf_pos == 4) {
            struct uart_event_t event = {
                .event_type = buf[1],
                .row = buf[2],
                .col = buf[3]
            };

            // Envia o evento para a fila
            if (k_msgq_put(&uart_event_msgq, &event, K_NO_WAIT) != 0) {
                printk("Fila cheia! Evento (%d,%d) perdido.\n", event.row, event.col);
            }

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
