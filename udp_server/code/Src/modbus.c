#include "modbus.h"
#include "usart.h"
#include "usart_ex.h"
#include "crc.h"

#define BUF_SIZE 128

enum modbus_func {
    MODBUS_FUNC_READ_HOLDING_REGS = 0x03,
    MODBUS_FUNC_READ_INPUT_REGS = 0x04,
    MODBUS_FUNC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FUNC_WRITE_SINGLE_REG = 0x06,
    MODBUS_FUNC_WRITE_MULTIPLE_REGS = 0x10,
};

enum modbus_state {
    STATE_IDLE,
    STATE_REQUEST_SEND,
    STATE_RESPONSE_RECEIVE,
} state;

uint8_t tx_buf[BUF_SIZE];
uint8_t rx_buf[BUF_SIZE];

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

struct modbus_regs {
    uint16_t addr;
    uint16_t quality;
};

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
    struct modbus_regs multi_regs;
    uint8_t byte_count;
    uint16_t regs[];
} __attribute__((packed));

struct modbus_frame_holding_regs {
    struct modbus_header header;
    struct modbus_regs regs;
} __attribute__((packed));

void modbus_init(void)
{
    MX_UART4_Init();
    MX_UART7_Init();
    CRC_Init();
}

void modbus_write_single_coil(uint8_t id, uint16_t addr, enum modbus_coil_state state)
{
    state = STATE_REQUEST_SEND;

    struct modbus_frame_coil *frame = (struct modbus_frame_coil *)tx_buf;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->coil.addr = __REV16(addr);
    frame->coil.state = __REV16(state);
    CRC_to_end_array_u16(tx_buf, sizeof(struct modbus_frame_coil) / 2);

    uart4_send_array_dma(tx_buf, sizeof(struct modbus_frame_coil) + CRC_SIZE);
}

void modbus_write_single_reg(uint8_t id, uint16_t addr, uint16_t val)
{
    state = STATE_REQUEST_SEND;

    struct modbus_frame_single_reg *frame = (struct modbus_frame_single_reg *)tx_buf;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    frame->reg.addr = __REV16(addr);
    frame->reg.val = __REV16(val);
    CRC_to_end_array_u16(tx_buf, sizeof(struct modbus_frame_single_reg) / 2);

    uart4_send_array_dma(tx_buf, sizeof(struct modbus_frame_single_reg) + CRC_SIZE);
    uart4_receive_array_dma(rx_buf, USART_MAX_BUF_SIZE);
}

void modbus_write_multi_regs(uint8_t id, uint16_t addr, uint16_t *data, uint16_t quality)
{
    struct modbus_frame_multi_regs *frame = (struct modbus_frame_multi_regs *)tx_buf;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_WRITE_MULTIPLE_REGS;
    frame->multi_regs.addr = __REV16(addr);
    frame->multi_regs.quality = __REV16(quality);
    frame->byte_count = quality * 2;

    uint32_t size = sizeof(struct modbus_frame_multi_regs) + quality * 2;

    uint16_t *frame_data = frame->regs;
    do {
        *frame_data++ = *data++;
    } while (--quality);
    CRC_to_end_array_u8(tx_buf, size);

    uart4_send_array_dma(tx_buf, size + CRC_SIZE);
}

void modbus_read_holding_regs(uint8_t id, uint16_t addr, uint16_t quality)
{
    struct modbus_frame_holding_regs *frame = (struct modbus_frame_holding_regs *)tx_buf;

    frame->header.id = id;
    frame->header.func = MODBUS_FUNC_READ_HOLDING_REGS;
    frame->regs.addr = __REV16(addr);
    frame->regs.quality = __REV16(quality);

    CRC_to_end_array_u16(tx_buf, sizeof(struct modbus_frame_holding_regs) / 2);

    uart4_send_array_dma(tx_buf, sizeof(struct modbus_frame_holding_regs) + CRC_SIZE);
}

void uart4_receive_callback(void *buf, uint32_t size)
{
    if (CRC_is_valid_u8(buf, size) == 0) {
        return;
    }

    struct modbus_header *header = (struct modbus_header *)buf;
    switch (header->func) {
    case MODBUS_FUNC_WRITE_SINGLE_COIL:
        break;
    case MODBUS_FUNC_WRITE_SINGLE_REG:
        break;
    case MODBUS_FUNC_WRITE_MULTIPLE_REGS:
        break;
    case MODBUS_FUNC_READ_HOLDING_REGS:
        break;

    default:
        break;
    }
}
