#ifndef __MODBUS_H
#define __MODBUS_H

#include "stm32f7xx.h"


enum modbus_coil_state {
    COIL_OFF = 0x0000,
    COIL_ON = 0x00FF
};

void modbus_init(void);
void modbus_write_single_coil(uint8_t id, uint16_t addr, enum modbus_coil_state state);
void modbus_write_single_reg(uint8_t id, uint16_t addr, uint16_t val);
void modbus_write_multi_reg(uint8_t id, uint16_t addr, uint16_t *data, uint16_t quality);

#endif
