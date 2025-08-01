#error "Verificando se uart_receiver.c está sendo compilado!"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>  // <- necessário para SYS_INIT()

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

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
        k_sem_give(&uart_data_sem);
    }
}

void uart_receiver_init(void)
{
    if (!device_is_ready(led.port)) return;
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    if (!device_is_ready(uart)) return;

    k_sem_init(&uart_data_sem, 0, 1);

    uart_irq_callback_user_data_set(uart, uart_cb, NULL);
    uart_irq_rx_enable(uart);

    k_thread_create(&led_thread_data, led_stack, K_THREAD_STACK_SIZEOF(led_stack),
                    led_blink_thread, NULL, NULL, NULL,
                    7, 0, K_NO_WAIT);
}

// Wrapper para SYS_INIT
static int uart_receiver_sys_init(const struct device *dev)
{
    ARG_UNUSED(dev);
    uart_receiver_init();
    return 0;
}

// Chama a função acima na inicialização do sistema
SYS_INIT(uart_receiver_sys_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
