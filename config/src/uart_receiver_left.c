/* uart_receiver_left.c - versão corrigida */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/uart_switch_left.h>
#include <zmk/uart_move_mouse.h>

LOG_MODULE_REGISTER(uart_receiver_left, LOG_LEVEL_INF);

/* UART device (ajuste se o nodelabel for diferente) */
static const struct device *uart_left = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Tipos (mesmo que no right) */
#define EVT_KEYBOARD 0x01
#define EVT_MOUSE    0x02

/* Pacote esperado: [0xAA][event_type][row][col][pressed][checksum] => 6 bytes */
static uint8_t uart_left_buf[7];
static int uart_left_buf_pos = 0;
static int uart_left_expected_len = 0;

/* Estrutura de evento (igual ao right, mas suporta mouse também) */
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
            uint8_t buttons;
        } mouse;
    };
};

#define UART_LEFT_EVENT_QUEUE_SIZE 32
K_MSGQ_DEFINE(uart_left_event_msgq, sizeof(struct uart_left_event_t), UART_LEFT_EVENT_QUEUE_SIZE, 4);

/* Thread */
K_THREAD_STACK_DEFINE(uart_left_stack, 1024);
static struct k_thread uart_left_thread_data;

/* Processa eventos vindos da fila */
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
            uart_mouse_move(event.mouse.dx, event.mouse.dy, event.mouse.buttons);
            break;

        default:
            LOG_WRN("evento desconhecido: %02x", event.event_type);
            break;
        }
    }
}

/* Callback UART - monta pacotes */
static void uart_left_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    ARG_UNUSED(user_data);

    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (uart_left_buf_pos == 0 && c != 0xAA) {
            continue; /* espera byte inicial */
        }

        if (uart_left_buf_pos < (int)sizeof(uart_left_buf)) {
            uart_left_buf[uart_left_buf_pos++] = c;
        } else {
            /* proteção: overflow */
            LOG_ERR("buffer overflow detectado, resetando");
            uart_left_buf_pos = 0;
            uart_left_expected_len = 0;
            continue;
        }

        /* Assim que lê o tipo de evento, define o tamanho esperado */
        if (uart_left_buf_pos == 2) {
            if (uart_left_buf[1] == EVT_KEYBOARD) {
                uart_left_expected_len = 6; /* [AA][type][row][col][pressed][checksum] */
            } else if (uart_left_buf[1] == EVT_MOUSE) {
                uart_left_expected_len = 6; /* [AA][type][dx][dy][buttons][checksum] */
            } else {
                /* tipo inválido */
                LOG_WRN("tipo inválido recebido: 0x%02x", uart_left_buf[1]);
                uart_left_buf_pos = 0;
                uart_left_expected_len = 0;
                continue;
            }
        }

        /* Se pacote completo chegou */
        if (uart_left_expected_len > 0 && uart_left_buf_pos == uart_left_expected_len) {
            uint8_t checksum = 0;
            for (int i = 1; i < uart_left_expected_len - 1; i++) {
                checksum ^= uart_left_buf[i];
            }

            if (checksum != uart_left_buf[uart_left_expected_len - 1]) {
                LOG_WRN("checksum inválido: esperado 0x%02x recebido 0x%02x",
                        checksum, uart_left_buf[uart_left_expected_len - 1]);
                uart_left_buf_pos = 0;
                uart_left_expected_len = 0;
                continue; /* pacote inválido */
            }

            struct uart_left_event_t event = { .event_type = uart_left_buf[1] };

            if (event.event_type == EVT_KEYBOARD) {
                event.key.row = uart_left_buf[2];
                event.key.col = uart_left_buf[3];
                event.key.pressed = uart_left_buf[4];
            } else if (event.event_type == EVT_MOUSE) {
                event.mouse.dx = (int8_t)uart_left_buf[2];
                event.mouse.dy = (int8_t)uart_left_buf[3];
                event.mouse.buttons = uart_left_buf[4];
            }

            int ret = k_msgq_put(&uart_left_event_msgq, &event, K_NO_WAIT);
            if (ret != 0) {
                LOG_ERR("fila cheia, evento descartado");
            }

            uart_left_buf_pos = 0;
            uart_left_expected_len = 0;
        }
    }
}

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

/* inicialização no boot (evitar colisão de nomes) */
SYS_INIT(uart_left_receiver_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
