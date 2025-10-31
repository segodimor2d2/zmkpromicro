#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/types.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

#include <zmk/split_mouse_service.h>
// #include <zmk/led_debug.h>

LOG_MODULE_REGISTER(split_mouse_svc, CONFIG_ZMK_LOG_LEVEL);

/*
 * UUID 128-bit custom para o serviço split mouse
 * Você pode alterar, mas mantenha fixo para os dois lados.
 */
#define BT_UUID_SPLIT_MOUSE_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12,0x34,0x56,0x78,0x90,0xab,0xcd, \
                       0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0)

#define BT_UUID_SPLIT_MOUSE_DATA_VAL \
    BT_UUID_128_ENCODE(0x21,0x43,0x65,0x87,0x09,0xba,0xdc, \
                       0xfe,0x21,0x43,0x65,0x87,0xa9,0xcb,0xed,0x0f)

static struct bt_uuid_128 split_mouse_service_uuid =
    BT_UUID_INIT_128(0xf0debc9a78563412ULL,
                     0x12efcdab90785634ULL);

static struct bt_uuid_128 split_mouse_data_uuid =
    BT_UUID_INIT_128(0x0fedcba987654321ULL,
                     0x21fedcba98765432ULL);

/* O valor que vamos enviar (máximo 20 bytes para MTU de BLE) */
static uint8_t split_mouse_value[20];

/* Serviço GATT */
BT_GATT_SERVICE_DEFINE(split_mouse_svc,
    BT_GATT_PRIMARY_SERVICE(&split_mouse_service_uuid),

    /* Characteristic: write + notify */
    BT_GATT_CHARACTERISTIC(&split_mouse_data_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           NULL, NULL, split_mouse_value),

    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

/* Função chamada pelo uart_move_mouse_right() */
int split_mouse_notify(const uint8_t *data, uint8_t len)
{
    if (len > sizeof(split_mouse_value)) {
        return -EINVAL;
    }

    memcpy(split_mouse_value, data, len);

    /* notify para todos os connections */
    int rc = bt_gatt_notify(NULL, &split_mouse_svc.attrs[2],
                            split_mouse_value, len);

    // if (rc == 0) {
    //     led_blink_pattern(1, 40);
    // }

    return rc;
}
