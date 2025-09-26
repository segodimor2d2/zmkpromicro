#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zmk/uart_switch_left.h>
#include <zmk/uart_move_mouse.h>

// UART device
static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

// Tipos de evento
#define EVT_KEYBOARD 0x01
#define EVT_MOUSE    0x02

// Estrutura de evento
struct uart_event_t {
    uint8_t event_type;
    union {
        struct {
            uint8_t row;
            uint8_t col;
            uint8_t pressed;
        } key;
        struct {
            int8_t dx;
            int8_t dy;
            uint8_t buttons;
        } mouse;
    };
};

// Buffer de recepção
static uint8_t buf[7];
static int buf_pos = 0;
static int expected_len = 0;

// Fila de eventos
#define UART_EVENT_QUEUE_SIZE 32
K_MSGQ_DEFINE(uart_event_msgq, sizeof(struct uart_event_t), UART_EVENT_QUEUE_SIZE, 4);

// Thread para processar eventos UART
K_THREAD_STACK_DEFINE(uart_stack, 1024);
static struct k_thread uart_thread_data;

// Processa eventos vindos da fila
void uart_event_thread(void *a, void *b, void *c)
{
    struct uart_event_t event;

    while (1) {
        k_msgq_get(&uart_event_msgq, &event, K_FOREVER);

        switch (event.event_type) {
        case EVT_KEYBOARD:
            uart_switch_simulate_left(
                event.key.row,
                event.key.col,
                event.key.pressed ? true : false
            );
            break;

        case EVT_MOUSE:
            uart_mouse_move(event.mouse.dx, event.mouse.dy, event.mouse.buttons);
            break;
        }
    }
}

// Callback UART para montagem dos pacotes
static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (buf_pos == 0 && c != 0xAA) {
            continue; // espera byte inicial
        }

        buf[buf_pos++] = c;

        // Assim que lê o tipo de evento, define o tamanho esperado
        if (buf_pos == 2) {
            if (buf[1] == EVT_KEYBOARD) {
                expected_len = 6; // [AA][type][row][col][pressed][checksum]
            } else if (buf[1] == EVT_MOUSE) {
                expected_len = 6; // [AA][type][dx][dy][buttons][checksum]
            } else {
                buf_pos = 0; // tipo inválido
                continue;
            }
        }

        // Se pacote completo chegou
        if (expected_len > 0 && buf_pos == expected_len) {
            uint8_t checksum = 0;
            for (int i = 1; i < expected_len - 1; i++) {
                checksum ^= buf[i];
            }

            if (checksum != buf[expected_len - 1]) {
                buf_pos = 0;
                expected_len = 0;
                continue; // pacote inválido
            }

            struct uart_event_t event = { .event_type = buf[1] };

            if (event.event_type == EVT_KEYBOARD) {
                event.key.row = buf[2];
                event.key.col = buf[3];
                event.key.pressed = buf[4];
            } else if (event.event_type == EVT_MOUSE) {
                event.mouse.dx = (int8_t)buf[2];
                event.mouse.dy = (int8_t)buf[3];
                event.mouse.buttons = buf[4];
            }

            k_msgq_put(&uart_event_msgq, &event, K_NO_WAIT);

            buf_pos = 0;
            expected_len = 0;
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
