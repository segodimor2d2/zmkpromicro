#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/uart_switch_left.h>
#include <zmk/events/sensor_event.h>
#include <zmk/events/position_state_changed.h>

#include <zmk/events/split_peripheral_status_changed.h>

LOG_MODULE_REGISTER(debug_ble, CONFIG_ZMK_LOG_LEVEL);


#define MATRIX_COLS 12
#define ZMK_KEYMAP_POSITION(row, col) ((row) * MATRIX_COLS + (col))

// Declaração (protótipo)
// void uart_switch_simulate_left(uint8_t row, uint8_t col, bool press);

// Função auxiliar para enviar uma tecla
static void send_key(uint8_t row, uint8_t col) {
    uart_switch_simulate_left(row, col, true);   // Pressionar
    k_msleep(20);
    uart_switch_simulate_left(row, col, false);  // Soltar
    k_msleep(20);
}

static int handle_all_events(const zmk_event_t *eh) {

    send_key(2, 1);//z

    if (as_zmk_sensor_event(eh)) {
        send_key(2, 1);//z
        // LOG_INF(" [BLE DEBUG] Sensor event detected");
    } else if (as_zmk_position_state_changed(eh)) {
        send_key(2, 2);//x
        // LOG_INF(" [BLE DEBUG] Position state changed event detected");
    } else if (as_zmk_split_peripheral_status_changed(eh)) {
        send_key(2, 3);//c
        // LOG_INF(" [BLE DEBUG] Peripheral status event detected");
    } else {
        send_key(2, 5);//b
        // LOG_INF(" [BLE DEBUG] Unknown event type");
    }

    return ZMK_EV_EVENT_BUBBLE; // permite outros listeners processarem
}


ZMK_LISTENER(debug_ble_events, handle_all_events);
ZMK_SUBSCRIPTION(debug_ble_events, zmk_sensor_event);
// ZMK_SUBSCRIPTION(debug_ble_events, zmk_position_state_changed);
ZMK_SUBSCRIPTION(debug_ble_events, zmk_split_peripheral_status_changed);

//
// #if IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
// ZMK_SUBSCRIPTION(debug_ble_events, zmk_split_central_status_changed);
// #endif
