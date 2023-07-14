#include "modbus.h"
#include "usart.h"
#include "usart_ex.h"
#include "crc.h"

#define BUF_SIZE 128

enum modbus_func {
    MODBUS_FUNC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FUNC_WRITE_SINGLE_REG = 0x06,
    MODBUS_FUNC_WRITE_MULTIPLE_REG = 0x10,
};

struct modbus_header {
    uint8_t id;
    uint8_t func;
};

uint8_t raw[BUF_SIZE];

struct modbus_coil {
    uint16_t addr;
    uint16_t state;
};

struct modbus_reg {
    uint16_t addr;
    uint16_t val;
};

struct modbus_coil_frame {
    struct modbus_header header;
    struct modbus_coil coil;
    uint16_t crc;
};

struct modbus_reg_frame {
    struct modbus_header header;
    struct modbus_reg reg;
    uint16_t crc;
};

// Нет поля CRC, так неизвестно сколько регистров
struct modbus_multi_reg_frame {
    struct modbus_header header;
    uint16_t addr;
    uint16_t quality;
    uint8_t byte_count;
    uint16_t reg[];
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
    struct modbus_coil_frame *frame = (struct modbus_coil_frame *)&raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->coil.addr = __REV16(addr);
    frame->coil.state = __REV16(state);
    uint16_t crc = CRC_calc_u16(
        frame,
        (sizeof(struct modbus_header) + sizeof(struct modbus_coil)) / 2);
    frame->crc = __REV16(crc);

    uart4_send_array_dma(&raw, sizeof(struct modbus_coil_frame));
}

void modbus_write_single_reg(uint8_t id, uint16_t addr, uint16_t val)
{
    struct modbus_reg_frame *frame = (struct modbus_reg_frame *)&raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->reg.addr = __REV16(addr);
    frame->reg.val = __REV16(val);
    uint16_t crc = CRC_calc_u16(
        frame,
        (sizeof(struct modbus_header) + sizeof(struct modbus_reg)) / 2);
    frame->crc = __REV16(crc);

    uart4_send_array_dma(&raw, sizeof(struct modbus_reg_frame));
}

void modbus_write_multi_reg(uint8_t id, uint16_t addr, uint16_t *data, uint16_t quality)
{
    struct modbus_multi_reg_frame *frame = (struct modbus_multi_reg_frame *)&raw;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_MULTIPLE_REG;
    frame->addr = __REV16(addr);
    frame->quality = __REV16(quality);
    frame->byte_count = quality * 2;

    uint32_t size = 7 + quality * 2;

    uint16_t *frame_data = frame->reg;
    do {
        *frame_data++ = *data++;
    } while (--quality);

    uint16_t crc = CRC_calc_u8(frame, size);
    *frame_data = __REV16(crc);

    uart4_send_array_dma(&raw, size + 2);
}
