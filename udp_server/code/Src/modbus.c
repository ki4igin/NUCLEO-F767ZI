#include "modbus.h"
#include "usart.h"
#include "usart_ex.h"
#include "crc.h"

#define BUF_SIZE 128

enum modbus_func {
    MODBUS_FUNC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FUNC_WRITE_SINGLE_REG = 0x06,
    MODBUS_FUNC_WRITE_MULTIPLE_REGS = 0x10,
};

uint8_t raw[BUF_SIZE];

struct modbus_header {
    uint8_t id;
    uint8_t func;
};

struct modbus_coil {
    uint16_t addr;
    uint16_t state;
};

struct modbus_reg {
    uint16_t addr;
    uint16_t val;
};

struct modbus_multi_regs {
    uint16_t addr;
    uint16_t quality;
    uint8_t byte_count;
} __attribute__((packed));

struct modbus_frame_coil {
    struct modbus_header header;
    struct modbus_coil coil;
};

struct modbus_frame_single_reg {
    struct modbus_header header;
    struct modbus_reg reg;
};

struct modbus_frame_multi_regs {
    struct modbus_header header;
    struct modbus_multi_regs multi_regs;    
    uint16_t regs[];
} __attribute__((packed));

void modbus_init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    MX_UART4_Init();
    MX_UART7_Init();
    CRC_Init();
}

void modbus_write_single_coil(uint8_t id, uint16_t addr, enum modbus_coil_state state)
{
    struct modbus_frame_coil *frame = (struct modbus_frame_coil *)raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->coil.addr = __REV16(addr);
    frame->coil.state = __REV16(state);
    CRC_to_end_array_u16(raw, sizeof(struct modbus_frame_coil) / 2);

    uart4_send_array_dma(raw, sizeof(struct modbus_frame_coil) + CRC_SIZE);
}

void modbus_write_single_reg(uint8_t id, uint16_t addr, uint16_t val)
{
    struct modbus_frame_single_reg *frame = (struct modbus_frame_single_reg *)raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->reg.addr = __REV16(addr);
    frame->reg.val = __REV16(val);
    CRC_to_end_array_u16(raw, sizeof(struct modbus_frame_single_reg) / 2);

    uart4_send_array_dma(raw, sizeof(struct modbus_frame_single_reg) + CRC_SIZE);
}

void modbus_write_multi_regs(uint8_t id, uint16_t addr, uint16_t *data, uint16_t quality)
{
    struct modbus_frame_multi_regs *frame = (struct modbus_frame_multi_regs *)raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_MULTIPLE_REGS;
    frame->multi_regs.addr = __REV16(addr);
    frame->multi_regs.quality = __REV16(quality);
    frame->multi_regs.byte_count = quality * 2;

    uint32_t size = sizeof(struct modbus_frame_multi_regs) + quality * 2;

    uint16_t *frame_data = frame->regs;
    do {
        *frame_data++ = *data++;
    } while (--quality);
    CRC_to_end_array_u8(raw, size);

    uart4_send_array_dma(raw, size + CRC_SIZE);
}
