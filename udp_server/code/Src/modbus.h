
#ifndef __MODBUS_H
#define __MODBUS_H

#include "stm32f7xx.h"

enum modbus_coil_state {
    COIL_OFF = 0x0000,
    COIL_ON = 0xFF00
};

struct modbus_out {
    uint16_t status;
    uint16_t quality;
    uint16_t regs[8];
};

typedef void (*modbus_recv_fn)(void);

extern struct modbus modbus1, modbus2;

inline static struct modbus_out *modbus_get_out(struct modbus *modbus)
{
    return (struct modbus_out *)modbus;
}

void modbus_init(void);
void modbus_recv(struct modbus *modbus, modbus_recv_fn recv);
void modbus_write_single_coil(struct modbus *modbus, uint16_t addr, enum modbus_coil_state state);
void modbus_write_single_reg(struct modbus *modbus, uint16_t addr, uint16_t val);
void modbus_write_multi_regs(struct modbus *modbus, uint16_t addr, uint16_t *data, uint16_t quality);
void modbus_read_holding_regs(struct modbus *modbus, uint16_t addr, uint16_t quality);
void modbus_read_input_regs(struct modbus *modbus, uint16_t addr, uint16_t quality);

#endif
