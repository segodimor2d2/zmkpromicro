/* uart_receiver_left.c */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/uart_switch_left.h>
#include <zmk/uart_move_mouse_left.h>

LOG_MODULE_REGISTER(uart_receiver_left, LOG_LEVEL_INF);

/* UART device */
static const struct device *uart_left = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Tipos de evento */
#define EVT_KEYBOARD 0x01
#define EVT_MOUSE    0x02

/* Buffer UART */
static uint8_t uart_left_buf[16];
static int uart_left_buf_pos = 0;
static int uart_left_expected_len = 0;

/* Estrutura de evento */
struct uart_left_event_t {
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
            int8_t scroll_y;
            int8_t scroll_x;
            uint8_t buttons;
        } mouse;
    };
};

/* Fila de eventos */
#define UART_LEFT_EVENT_QUEUE_SIZE 32
K_MSGQ_DEFINE(uart_left_event_msgq, sizeof(struct uart_left_event_t), UART_LEFT_EVENT_QUEUE_SIZE, 4);

/* Thread stack */
K_THREAD_STACK_DEFINE(uart_left_stack, 1024);
static struct k_thread uart_left_thread_data;

/* Thread de processamento */
void uart_left_event_thread(void *a, void *b, void *c)
{
    struct uart_left_event_t event;

    while (1) {
        k_msgq_get(&uart_left_event_msgq, &event, K_FOREVER);

        switch (event.event_type) {
        case EVT_KEYBOARD:
            uart_switch_simulate_left(
                event.key.row,
                event.key.col,
                event.key.pressed ? true : false
            );
            break;

        case EVT_MOUSE:
            uart_move_mouse_left(
                event.mouse.dx,
                event.mouse.dy,
                event.mouse.scroll_y,
                event.mouse.scroll_x,
                event.mouse.buttons
            );
            break;

        default:
            LOG_WRN("Evento desconhecido: %02x", event.event_type);
            break;
        }
    }
}

/* Callback UART */
static void uart_left_cb(const struct device *dev, void *user_data)
{
    uint8_t c;
    ARG_UNUSED(user_data);

    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (uart_left_buf_pos == 0 && c != 0xAA) {
            continue; // espera byte inicial
        }

        if (uart_left_buf_pos < (int)sizeof(uart_left_buf)) {
            uart_left_buf[uart_left_buf_pos++] = c;
        } else {
            LOG_ERR("Buffer overflow detectado, resetando");
            uart_left_buf_pos = 0;
            uart_left_expected_len = 0;
            continue;
        }

        /* Define tamanho esperado */
        if (uart_left_buf_pos == 2) {
            if (uart_left_buf[1] == EVT_KEYBOARD) {
                uart_left_expected_len = 6; // [AA][type][row][col][pressed][checksum]
            } else if (uart_left_buf[1] == EVT_MOUSE) {
                uart_left_expected_len = 8; // [AA][type][dx][dy][scrollY][scrollX][buttons][checksum]
            } else {
                LOG_WRN("Tipo inválido recebido: 0x%02x", uart_left_buf[1]);
                uart_left_buf_pos = 0;
                uart_left_expected_len = 0;
                continue;
            }
        }

        /* Pacote completo */
        if (uart_left_expected_len > 0 && uart_left_buf_pos == uart_left_expected_len) {
            /* Checksum */
            uint8_t checksum = 0;
            for (int i = 1; i < uart_left_expected_len - 1; i++) {
                checksum ^= uart_left_buf[i];
            }

            if (checksum != uart_left_buf[uart_left_expected_len - 1]) {
                LOG_WRN("Checksum inválido: esperado 0x%02x recebido 0x%02x",
                        checksum, uart_left_buf[uart_left_expected_len - 1]);
                uart_left_buf_pos = 0;
                uart_left_expected_len = 0;
                continue;
            }

            /* Cria evento */
            struct uart_left_event_t event = { .event_type = uart_left_buf[1] };

            if (event.event_type == EVT_KEYBOARD) {
                event.key.row = uart_left_buf[2];
                event.key.col = uart_left_buf[3];
                event.key.pressed = uart_left_buf[4];
            } else if (event.event_type == EVT_MOUSE) {
                event.mouse.dx       = (int8_t)uart_left_buf[2];
                event.mouse.dy       = (int8_t)uart_left_buf[3];
                event.mouse.scroll_y = (int8_t)uart_left_buf[4];
                event.mouse.scroll_x = (int8_t)uart_left_buf[5];
                event.mouse.buttons  = uart_left_buf[6];
            }

            int ret = k_msgq_put(&uart_left_event_msgq, &event, K_NO_WAIT);
            if (ret != 0) {
                LOG_ERR("Fila cheia, evento descartado");
            }

            uart_left_buf_pos = 0;
            uart_left_expected_len = 0;
        }
    }
}

/* Inicializa receptor UART */
void uart_left_receiver_init(void)
{
    if (!device_is_ready(uart_left)) {
        LOG_ERR("UART device not ready");
        return;
    }

    uart_irq_callback_user_data_set(uart_left, uart_left_cb, NULL);
    uart_irq_rx_enable(uart_left);

    k_thread_create(&uart_left_thread_data, uart_left_stack,
                    K_THREAD_STACK_SIZEOF(uart_left_stack),
                    uart_left_event_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);

    LOG_INF("uart_receiver_left init done");
}

static int uart_left_receiver_sys_init(void)
{
    uart_left_receiver_init();
    return 0;
}

SYS_INIT(uart_left_receiver_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
