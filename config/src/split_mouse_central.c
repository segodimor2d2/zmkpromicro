#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/types.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>

#include <zmk/led_debug.h>
#include <zmk/uart_move_mouse_left.h>

LOG_MODULE_REGISTER(split_mouse_central, CONFIG_ZMK_LOG_LEVEL);

/* UUIDs (iguais ao peripheral, versão 128-bit em uint64_t) */
static struct bt_uuid_128 split_mouse_service_uuid =
    BT_UUID_INIT_128(0xf0debc9a78563412ULL,
                     0x12efcdab90785634ULL);

static struct bt_uuid_128 split_mouse_data_uuid =
    BT_UUID_INIT_128(0x0fedcba987654321ULL,
                     0x21fedcba98765432ULL);

/* Discovery + Subscribe params */
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;

/* Callback de notify (recebe payload do peripheral) */
static uint8_t split_mouse_notify_cb(struct bt_conn *conn,
                                     struct bt_gatt_subscribe_params *params,
                                     const void *data, uint16_t len)
{
    if (!data || len < 6) {
        return BT_GATT_ITER_CONTINUE;
    }

    const uint8_t *buf = data;

    /* Nosso tipo de pacote */
    if (buf[0] != 0x02) {
        return BT_GATT_ITER_CONTINUE;
    }

    int8_t dx        = (int8_t)buf[1];
    int8_t dy        = (int8_t)buf[2];
    int8_t scroll_y  = (int8_t)buf[3];
    int8_t scroll_x  = (int8_t)buf[4];
    uint8_t buttons  = buf[5];

    uart_move_mouse_left(dx, dy, scroll_y, scroll_x, buttons);

    led_blink_pattern(1, 60);

    return BT_GATT_ITER_CONTINUE;
}

/* Descoberta da characteristic via UUID */
static uint8_t split_mouse_discover_func(struct bt_conn *conn,
                                         const struct bt_gatt_attr *attr,
                                         struct bt_gatt_discover_params *params)
{
    if (!attr) {
        led_blink_pattern(3, 200);
        LOG_WRN("split_mouse: discovery finished (not found)");
        memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }


    if (!bt_uuid_cmp(params->uuid, &split_mouse_data_uuid.uuid)) {
        LOG_INF("split_mouse: characteristic found, handle=0x%x", attr->handle);

        led_blink_pattern(2, 60);

        subscribe_params.notify     = split_mouse_notify_cb;
        subscribe_params.value      = BT_GATT_CCC_NOTIFY;
        subscribe_params.ccc_handle = attr->handle + 2;
        subscribe_params.end_handle = 0xffff;
        subscribe_params.disc_params = NULL;

        int rc = bt_gatt_subscribe(conn, &subscribe_params);
        if (rc == 0) {
            led_blink_pattern(4, 60);  // subscribe OK
        } else {
            led_blink_pattern(5, 200); // subscribe falhou!
        }
        LOG_INF("split_mouse: subscribe rc=%d", rc);

        memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void split_mouse_start_discovery(struct bt_conn *conn)
{
    memset(&discover_params, 0, sizeof(discover_params));

    discover_params.uuid         = &split_mouse_data_uuid.uuid;
    discover_params.func         = split_mouse_discover_func;
    discover_params.start_handle = 0x0001;
    discover_params.end_handle   = 0xffff;
    discover_params.type         = BT_GATT_DISCOVER_CHARACTERISTIC;

    bt_gatt_discover(conn, &discover_params);
}

/* ---------- CORREÇÃO REAL: substituir lambda por função C ---------- */

static void split_mouse_connected(struct bt_conn *conn, uint8_t err)
{
    led_blink_pattern(10, 40); // indica conexão BLE OK
    if (err == 0) {
        split_mouse_start_discovery(conn);
    }
}

/* Registro de callbacks BT */
static struct bt_conn_cb conn_callbacks = {
    .connected = split_mouse_connected,
};

/* Init automático no boot */
static int split_mouse_central_init(void)
{
    bt_conn_cb_register(&conn_callbacks);
    return 0;
}

SYS_INIT(split_mouse_central_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
