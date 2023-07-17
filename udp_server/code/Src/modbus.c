#include "modbus.h"
#include "usart.h"
#include "usart_ex.h"
#include "crc.h"
#include "assert.h"
#include "debug.h"

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

struct modbus_regs_header {
    uint16_t addr;
    uint16_t quality;
};

union modbus_request {
    struct {
        struct modbus_header header;

        union {
            struct modbus_coil coil;

            struct modbus_reg reg;

            struct modbus_multi_regs {
                struct modbus_regs_header header;
                uint8_t byte_count;
                uint16_t regs[];
            } __attribute__((packed)) multi_regs;

            struct modbus_regs_header holding_regs;

            struct modbus_regs_header input_regs;
        };
    };

    uint8_t raw[BUF_SIZE];
};

union modbus_response {
    struct {
        struct modbus_header header;

        union {
            struct {
                uint8_t byte_count;
                uint16_t regs[];
            } __attribute__((packed)) holding_regs;

            struct {
                uint8_t byte_count;
                uint16_t regs[];
            } __attribute__((packed)) input_regs;

            struct modbus_coil coil;

            struct modbus_reg reg;

            struct modbus_regs_header multi_regs;
        };
    };

    uint8_t raw[BUF_SIZE];
};

struct modbus {
    struct modbus_out out;
    union modbus_request request;
    union modbus_response response;
    void (*send_request)(void *raw, uint32_t size);
    void (*receive_response)(void *raw, uint32_t size);
} modbus1 = {
    .request.header.id = 0x01,
    .send_request = uart4_send_array_dma,
    .receive_response = uart4_receive_array_dma,
},
  modbus2 = {
      .request.header.id = 0x01,
      .send_request = uart7_send_array_dma,
      .receive_response = uart7_receive_array_dma,
};

// static_assert(sizeof(struct modbus_multi_regs) == 5, "WTF");

void modbus_init(void)
{
    MX_UART4_Init();
    MX_UART7_Init();
    CRC_Init();
}

void modbus_write_single_coil(struct modbus *modbus, uint16_t addr, enum modbus_coil_state state)
{
    modbus->request.header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    modbus->request.coil.addr = __REV16(addr);
    modbus->request.coil.state = __REV16(state);
    uint32_t size = sizeof(struct modbus_header) + sizeof(struct modbus_coil);
    CRC_to_end_array_u16(modbus->request.raw, size / 2);
    modbus->send_request(modbus->request.raw, size + CRC_SIZE);
    modbus->receive_response(modbus->response.raw, USART_MAX_BUF_SIZE);
}

void modbus_write_single_reg(struct modbus *modbus, uint16_t addr, uint16_t val)
{
    modbus->request.header.func = MODBUS_FUNC_WRITE_SINGLE_COIL;
    modbus->request.reg.addr = __REV16(addr);
    modbus->request.reg.val = __REV16(val);
    uint32_t size = sizeof(struct modbus_header) + sizeof(struct modbus_reg);
    CRC_to_end_array_u16(modbus->request.raw, size / 2);
    modbus->send_request(modbus->request.raw, size + CRC_SIZE);
    modbus->receive_response(modbus->response.raw, USART_MAX_BUF_SIZE);
}

void modbus_write_multi_regs(struct modbus *modbus, uint16_t addr, uint16_t *data, uint16_t quality)
{
    debug_printf("modbus: write multi regs\n");

    modbus->request.header.func = MODBUS_FUNC_WRITE_MULTIPLE_REGS;
    modbus->request.multi_regs.header.addr = __REV16(addr);
    modbus->request.multi_regs.header.quality = __REV16(quality);
    modbus->request.multi_regs.byte_count = quality * 2;
    uint32_t size = sizeof(struct modbus_header)
                  + sizeof(struct modbus_multi_regs)
                  + quality * 2;
    uint16_t *frame_data = modbus->request.multi_regs.regs;
    do {
        *frame_data++ = *data++;
    } while (--quality);
    CRC_to_end_array_u8(modbus->request.raw, size);
    modbus->send_request(modbus->request.raw, size + CRC_SIZE);
    modbus->receive_response(modbus->response.raw, USART_MAX_BUF_SIZE);
}

void modbus_read_holding_regs(struct modbus *modbus, uint16_t addr, uint16_t quality)
{
    modbus->request.header.func = MODBUS_FUNC_READ_HOLDING_REGS;
    modbus->request.holding_regs.addr = __REV16(addr);
    modbus->request.holding_regs.quality = __REV16(quality);
    uint32_t size = sizeof(struct modbus_header)
                  + sizeof(struct modbus_regs_header);
    CRC_to_end_array_u16(modbus->request.raw, size / 2);
    modbus->send_request(modbus->request.raw, size + CRC_SIZE);
    modbus->receive_response(modbus->response.raw, USART_MAX_BUF_SIZE);
}

void modbus_read_input_regs(struct modbus *modbus, uint16_t addr, uint16_t quality)
{
    modbus->request.header.func = MODBUS_FUNC_READ_INPUT_REGS;
    modbus->request.input_regs.addr = __REV16(addr);
    modbus->request.input_regs.quality = __REV16(quality);
    uint32_t size = sizeof(struct modbus_header)
                  + sizeof(struct modbus_regs_header);
    CRC_to_end_array_u16(modbus->request.raw, size / 2);
    modbus->send_request(modbus->request.raw, size + CRC_SIZE);
    modbus->receive_response(modbus->response.raw, USART_MAX_BUF_SIZE);
}

void modbus_response_callback(struct modbus *modbus, uint32_t size)
{
    if (CRC_is_valid_u8(modbus->response.raw, size) == 0) {
        return;
    }

    switch (modbus->response.header.func) {
    case MODBUS_FUNC_WRITE_SINGLE_COIL:
        // debug_printf("MODBUS_FUNC_WRITE_SINGLE_COIL\n");
        break;
    case MODBUS_FUNC_WRITE_SINGLE_REG:
        // debug_printf("MODBUS_FUNC_WRITE_SINGLE_REG\n");
        break;
    case MODBUS_FUNC_WRITE_MULTIPLE_REGS:
        // debug_printf("MODBUS_FUNC_WRITE_MULTIPLE_REGS\n");
        break;
    case MODBUS_FUNC_READ_HOLDING_REGS: {
        debug_printf("modbus: read hold regs");
        uint32_t count = modbus->response.holding_regs.byte_count * 2;
        modbus->out.quality = count;
        for (uint32_t i = 0; i < count; i++) {
            debug_printf("0x%x\n", __REV16(modbus->response.holding_regs.regs[i]));
            modbus->out.regs[i] = __REV16(modbus->response.holding_regs.regs[i]);
        }
        modbus->out.status = 1;
    } break;
    case MODBUS_FUNC_READ_INPUT_REGS: {
        uint32_t count = modbus->response.input_regs.byte_count * 2;
        modbus->out.quality = count;
        for (uint32_t i = 0; i < count; i++) {
            modbus->out.regs[i] = __REV16(modbus->response.input_regs.regs[i]);
        }
        modbus->out.status = 1;
    } break;

    default:
        break;
    }
}

void uart4_receive_callback(void *buf, uint32_t size)
{
    modbus_response_callback(&modbus1, size);
}

void uart7_receive_callback(void *buf, uint32_t size)
{
    modbus_response_callback(&modbus2, size);
}
