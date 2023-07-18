#include "motor.h"
#include "motor_regs.h"
#include "modbus.h"
#include "debug.h"

#define modbus_az modbus1
#define modbus_el modbus2

enum motor_state {
    STATE_IDLE,
    STATE_SET_HR_OFFSET,
    STATE_SET_C_START,
} state_az = STATE_IDLE, state_el = STATE_IDLE;

static void az_recv_callback(void)
{
    switch (state_az) {
    case STATE_SET_HR_OFFSET: {
        modbus_write_single_coil(&modbus_az, C_START, COIL_ON);
        state_az = STATE_SET_C_START;
    } break;
    case STATE_SET_C_START: {
        state_az = STATE_IDLE;
    } break;

    default:
        break;
    }
}

static void el_recv_callback(void)
{
    switch (state_el) {
    case STATE_SET_HR_OFFSET: {
        modbus_write_single_coil(&modbus_el, C_START, COIL_ON);
        state_el = STATE_SET_C_START;
    } break;
    case STATE_SET_C_START: {
        state_el = STATE_IDLE;
    } break;

    default:
        break;
    }
}

void motor_init(void)
{
    modbus_recv(&modbus_az, az_recv_callback);
    modbus_recv(&modbus_el, el_recv_callback);
    modbus_write_single_reg(&modbus_az, HR_MODE_DEVICE, 1);
    modbus_write_single_reg(&modbus_el, HR_MODE_DEVICE, 1);
    delay_ms(5);
    modbus_write_single_reg(&modbus_az, HR_MODE_ROTATION, 2);
    modbus_write_single_reg(&modbus_el, HR_MODE_ROTATION, 2);
}

void motor_az_offset(float delta)
{
    if (state_az != STATE_IDLE) {
        return;
    }

    int32_t offset = __REV((int32_t)(delta / 180 * INT32_MAX));
    modbus_write_multi_regs(&modbus_az, HR_OFFSET, (uint16_t *)&offset, 2);
    state_az = STATE_SET_HR_OFFSET;
}

void motor_el_offset(float delta)
{
    if (state_el != STATE_IDLE) {
        return;
    }

    int32_t offset = __REV((int32_t)(delta / 180 * INT32_MAX));
    modbus_write_multi_regs(&modbus_el, HR_OFFSET, (uint16_t *)&offset, 2);
    state_el = STATE_SET_HR_OFFSET;
}
