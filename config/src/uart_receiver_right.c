/* uart_receiver_right.c - versão corrigida */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zmk/uart_switch_right.h>

LOG_MODULE_REGISTER(uart_receiver_right, LOG_LEVEL_INF);

/* UART device (ajuste se o nodelabel for diferente) */
static const struct device *uart_right = DEVICE_DT_GET(DT_NODELABEL(uart0));

/* Tipos (mesmo que no left) */
#define EVT_KEYBOARD 0x01
#define EVT_MOUSE    0x02

/* Pacote esperado: [0xAA][event_type][row][col][pressed][checksum] => 6 bytes */
static uint8_t buf_right[7];
static int buf_pos_right = 0;
static int expected_len_right = 0;

/* Estrutura de evento (parecida com left, mas apenas teclado aqui) */
struct uart_event_right_t {
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

#define UART_EVENT_QUEUE_SIZE_RIGHT 32
K_MSGQ_DEFINE(uart_event_msgq_right, sizeof(struct uart_event_right_t), UART_EVENT_QUEUE_SIZE_RIGHT, 4);

/* Thread */
K_THREAD_STACK_DEFINE(uart_stack_right, 1024);
static struct k_thread uart_thread_data_right;

/* Processa eventos vindos da fila */
void uart_event_thread_right(void *a, void *b, void *c)
{
    struct uart_event_right_t event;

    while (1) {
        k_msgq_get(&uart_event_msgq_right, &event, K_FOREVER);

        switch (event.event_type) {
        case EVT_KEYBOARD:
            uart_switch_simulate_right(
                event.key.row,
                event.key.col,
                event.key.pressed ? true : false
            );
            break;

        case EVT_MOUSE:
            /* se precisar suportar mouse também, habilitar chamada apropriada */
            break;

        default:
            LOG_WRN("evento desconhecido: %02x", event.event_type);
            break;
        }
    }
}

/* Callback UART - monta pacotes */
static void uart_cb_right(const struct device *dev, void *user_data)
{
    uint8_t c;

    ARG_UNUSED(user_data);

    while (uart_fifo_read(dev, &c, 1) > 0) {
        if (buf_pos_right == 0 && c != 0xAA) {
            continue; /* espera byte inicial */
        }

        if (buf_pos_right < (int)sizeof(buf_right)) {
            buf_right[buf_pos_right++] = c;
        } else {
            /* proteção: se por algum motivo overflow, reset */
            LOG_ERR("buffer overflow detectado, resetando");
            buf_pos_right = 0;
            expected_len_right = 0;
            continue;
        }

        /* Assim que lê o tipo de evento, define o tamanho esperado */
        if (buf_pos_right == 2) {
            if (buf_right[1] == EVT_KEYBOARD) {
                expected_len_right = 6; /* [AA][type][row][col][pressed][checksum] */
            } else if (buf_right[1] == EVT_MOUSE) {
                expected_len_right = 6; /* se usar mouse com 3 bytes de payload */
            } else {
                /* tipo inválido */
                LOG_WRN("tipo inválido recebido: 0x%02x", buf_right[1]);
                buf_pos_right = 0;
                expected_len_right = 0;
                continue;
            }
        }

        /* Se pacote completo chegou */
        if (expected_len_right > 0 && buf_pos_right == expected_len_right) {
            uint8_t checksum = 0;
            /* XOR dos bytes de índice 1 até expected_len-2 (inclui pressed) */
            for (int i = 1; i < expected_len_right - 1; i++) {
                checksum ^= buf_right[i];
            }

            if (checksum != buf_right[expected_len_right - 1]) {
                LOG_WRN("checksum inválido: esperado 0x%02x recebido 0x%02x",
                        checksum, buf_right[expected_len_right - 1]);
                buf_pos_right = 0;
                expected_len_right = 0;
                continue; /* pacote inválido */
            }

            struct uart_event_right_t event = { .event_type = buf_right[1] };

            if (event.event_type == EVT_KEYBOARD) {
                event.key.row = buf_right[2];
                event.key.col = buf_right[3];
                event.key.pressed = buf_right[4];
            } else if (event.event_type == EVT_MOUSE) {
                event.mouse.dx = (int8_t)buf_right[2];
                event.mouse.dy = (int8_t)buf_right[3];
                event.mouse.buttons = buf_right[4];
            }

            int ret = k_msgq_put(&uart_event_msgq_right, &event, K_NO_WAIT);
            if (ret != 0) {
                LOG_ERR("fila cheia, evento descartado");
            }

            buf_pos_right = 0;
            expected_len_right = 0;
        }
    }
}

void uart_receiver_right_init(void)
{
    if (!device_is_ready(uart_right)) {
        LOG_ERR("UART device not ready");
        return;
    }

    uart_irq_callback_user_data_set(uart_right, uart_cb_right, NULL);
    uart_irq_rx_enable(uart_right);

    k_thread_create(&uart_thread_data_right, uart_stack_right,
                    K_THREAD_STACK_SIZEOF(uart_stack_right),
                    uart_event_thread_right, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);

    LOG_INF("uart_receiver_right init done");
}

static int uart_receiver_right_sys_init(void)
{
    uart_receiver_right_init();
    return 0;
}

/* inicialização no boot (evitar colisão de nomes) */
SYS_INIT(uart_receiver_right_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
