#include <zephyr/input/input.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/sensor_event.h>
#include <zmk/sensors.h>

#include <zmk/uart_switch_left.h>
#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_left(row, col, true);   // Pressionar
    k_msleep(20);
    uart_switch_simulate_left(row, col, false);  // Soltar
    k_msleep(20);
}

// Função para enviar o evento com dx e dy
static void send_mouse_motion(int8_t dx, int8_t dy) {
    struct zmk_sensor_channel_data data = {
        .value = {
            .val1 = dx,
            .val2 = dy,
        },
    };

    struct zmk_sensor_event event = {
        .channel_data_size = 1,
        .channel_data = {data},
        .timestamp = k_uptime_get(),
        .sensor_index = 0,
    };

    // raise_zmk_sensor_event(event);
    int ret = raise_zmk_sensor_event(event);
  
    switch (ret) {
        case 0:
            send_key(0, 1);//q
            break;
        case 1:
            send_key(0, 2);//w
            break;
        case -EINVAL:
            send_key(0, 3);//e
            break;
        case -ENOMEM:
            send_key(0, 4);//r
            break;
        case -ENOTSUP:
            send_key(0, 5);//t
            break;
        case -EAGAIN:
            send_key(0, 6);//y
            break;
        case -EFAULT:
            send_key(0, 7);//u
            break;
        default:
            send_key(0, 8);//i
            break;
    }
}

// Simulação periódica (exemplo)
static void test_mouse_thread(void) {
    while (true) {
        send_mouse_motion(5, -3); // exemplo de movimento
        k_msleep(1000);
    }
}

K_THREAD_DEFINE(test_mouse_thread_id, 1024, test_mouse_thread, NULL, NULL, NULL, 5, 0, 0);
