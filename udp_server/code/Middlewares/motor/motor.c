#include "motor.h"
#include "motor_regs.h"
#include "modbus.h"
#include "uart.h"
#include "debug.h"

static void az_recv_callback(struct modbus_out *out);
static void el_recv_callback(struct modbus_out *out);

enum motor_state {
    STATE_IDLE,
    STATE_SET_HR_OFFSET,
    STATE_SET_C_START,
};

struct motor {
    enum motor_state state;
    struct modbus modbus;
};

static struct motor az = {
    .state = STATE_IDLE,
    .modbus = {
        .req.head.id = 0x01,
        .send_req = uart4_send_array_dma,
        .recv_resp = uart4_recv_array_dma,
        .resp_callback = az_recv_callback,
    },
};

static struct motor el = {
    .state = STATE_IDLE,
    .modbus = {
        .req.head.id = 0x01,
        .send_req = uart7_send_array_dma,
        .recv_resp = uart7_recv_array_dma,
        .resp_callback = el_recv_callback,
    },
};

static void recv_callback(struct motor *motor, struct modbus_out *out)
{
    if (out->status == MODBUS_ERR_TIMEOUT) {
        return;
    }

    switch (motor->state) {
    case STATE_SET_HR_OFFSET: {
        modbus_write_single_coil(&motor->modbus, C_START, COIL_ON);
        motor->state = STATE_SET_C_START;
    } break;
    case STATE_SET_C_START: {
        motor->state = STATE_IDLE;
    } break;

    default:
        break;
    }
}

void uart4_recv_callback(uint32_t size)
{
    modbus_resp_working(&az.modbus, size);
}

void uart7_recv_callback(uint32_t size)
{
    modbus_resp_working(&el.modbus, size);
}

static void az_recv_callback(struct modbus_out *out)
{
    recv_callback(&az, out);
}

static void el_recv_callback(struct modbus_out *out)
{
    recv_callback(&el, out);
}

static void motor_offset(struct motor *motor, float delta)
{
    if (motor->state != STATE_IDLE) {
        return;
    }

    int32_t offset = __REV((int32_t)(delta / 180 * INT32_MAX));
    modbus_write_multi_regs(&motor->modbus, HR_OFFSET, (uint16_t *)&offset, 2);
    motor->state = STATE_SET_HR_OFFSET;
}

void motor_init(void)
{
    uart4_init();
    uart7_init();
    modbus_init();
    // modbus_write_single_reg(&modbus_az, HR_MODE_DEVICE, 1);
    // modbus_write_single_reg(&modbus_el, HR_MODE_DEVICE, 1);
    delay_ms(5);
    // modbus_write_single_reg(&modbus_az, HR_MODE_ROTATION, 2);
    // modbus_write_single_reg(&modbus_el, HR_MODE_ROTATION, 2);
}

void motor_az_offset(float delta)
{
    motor_offset(&az, delta);
}

void motor_el_offset(float delta)
{
    motor_offset(&el, delta);
}
