#include "motor.h"
#include "motor_regs.h"
#include "modbus.h"
#include "debug.h"

#define modbus_az modbus1
#define modbus_el modbus2

void motor_init(void)
{
    modbus_write_single_reg(&modbus_az, HR_MODE_DEVICE, 1);
    modbus_write_single_reg(&modbus_el, HR_MODE_DEVICE, 1);

    modbus_write_single_reg(&modbus_az, HR_MODE_ROTATION, 2);
    modbus_write_single_reg(&modbus_el, HR_MODE_ROTATION, 2);
}


void motor_az_offset(float delta)
{
    int32_t offset = __REV((int32_t)(delta / 180 * INT32_MAX));
    modbus_write_multi_regs(&modbus_az, HR_OFFSET, (uint16_t *)&offset, 2);
    delay_ms(5);
    modbus_write_single_coil(&modbus_az, C_START, COIL_ON);
}
