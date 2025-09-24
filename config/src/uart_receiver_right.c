#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zmk/uart_switch_right.h>

// UART device
static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

// Pacote UART: [0xAA][event_type][row][col][checksum]
static uint8_t buf[5];
static int buf_pos = 0;

// Estrutura para armazenar evento UART
struct uart_event_t {
    uint8_t event_type;
    uint8_t row;
    uint8_t col;
};

// Fila de eventos (aumentada para evitar perdas)
#define UART_EVENT_QUEUE_SIZE 32
K_MSGQ_DEFINE(uart_event_msgq, sizeof(struct uart_event_t), UART_EVENT_QUEUE_SIZE, 4);

// Stack e thread para processar eventos UART
K_THREAD_STACK_DEFINE(uart_stack, 1024);
static struct k_thread uart_thread_data;

void uart_event_thread(void *a, void *b, void *c)
{
    struct uart_event_t event;

    while (1) {
        k_msgq_get(&uart_event_msgq, &event, K_FOREVER);

        bool pressed = event.event_type == 0x01;
        uart_switch_simulate_right(event.row, event.col, pressed);
    }
}

static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (buf_pos == 0 && c != 0xAA) {
            continue; // espera byte inicial
        }

        buf[buf_pos++] = c;

        if (buf_pos == 5) {
            uint8_t event_type = buf[1];
            uint8_t row = buf[2];
            uint8_t col = buf[3];
            uint8_t checksum = buf[4];
            uint8_t expected_checksum = event_type ^ row ^ col;

            if (checksum != expected_checksum) {
                buf_pos = 0;
                continue;
            }

            struct uart_event_t event = {
                .event_type = event_type,
                .row = row,
                .col = col
            };

            k_msgq_put(&uart_event_msgq, &event, K_NO_WAIT);
            buf_pos = 0;
        }
    }
}

void uart_receiver_init(void)
{
    if (!device_is_ready(uart)) {
        return;
    }

    uart_irq_callback_user_data_set(uart, uart_cb, NULL);
    uart_irq_rx_enable(uart);

    k_thread_create(&uart_thread_data, uart_stack, K_THREAD_STACK_SIZEOF(uart_stack),
                    uart_event_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);
}

static int uart_receiver_sys_init(void)
{
    uart_receiver_init();
    return 0;
}

SYS_INIT(uart_receiver_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
