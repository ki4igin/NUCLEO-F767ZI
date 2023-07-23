#include "modbus.h"
#include "crc16.h"
#include "assert.h"
#include "debug.h"

enum modbus_func {
    FUNC_READ_HOLDING_REGS = 0x03,
    FUNC_READ_INPUT_REGS = 0x04,
    FUNC_WRITE_SINGLE_COIL = 0x05,
    FUNC_WRITE_SINGLE_REG = 0x06,
    FUNC_WRITE_MULTIPLE_REGS = 0x10,
};

void modbus_init(void)
{
    CRC16_Init();
}

static void start_resp(struct modbus *modbus, uint32_t size)
{
    CRC16_to_end_array(&modbus->req, size);
    modbus->send_req(&modbus->req, size + CRC16_SIZE);
    modbus->recv_resp(&modbus->resp, sizeof(modbus->resp));
}

void modbus_write_single_coil(struct modbus *modbus, uint16_t addr, enum modbus_coil_state state)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_SINGLE_COIL;
    req->coil.addr = __REVSH(addr);
    req->coil.state = __REVSH(state);
    uint32_t size = sizeof(req->head) + sizeof(req->coil);
    start_resp(modbus, size);
}

void modbus_write_single_reg(struct modbus *modbus, uint16_t addr, uint16_t val)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_SINGLE_REG;
    req->reg.addr = __REVSH(addr);
    req->reg.val = __REVSH(val);
    uint32_t size = sizeof(req->head) + sizeof(req->reg);
    start_resp(modbus, size);
}

void modbus_write_multi_regs(struct modbus *modbus, uint16_t addr, uint16_t *data, uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_MULTIPLE_REGS;
    req->multi_regs.addr = __REVSH(addr);
    req->multi_regs.quality = __REVSH(quality);
    req->multi_regs.byte_count = quality * 2;
    uint32_t size = sizeof(req->head) + sizeof(req->multi_regs)
                  - REGS_COUNT_MAX * 2 + quality * 2;
    uint16_t *frame_data = req->multi_regs.regs;
    do {
        *frame_data++ = *data++;
    } while (--quality);
    start_resp(modbus, size);
}

void modbus_read_holding_regs(struct modbus *modbus, uint16_t addr, uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_HOLDING_REGS;
    req->holding_regs.addr = __REVSH(addr);
    req->holding_regs.quality = __REVSH(quality);
    uint32_t size = sizeof(req->head) + sizeof(req->holding_regs);
    start_resp(modbus, size);
}

void modbus_read_input_regs(struct modbus *modbus, uint16_t addr, uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_INPUT_REGS;
    req->input_regs.addr = __REVSH(addr);
    req->input_regs.quality = __REVSH(quality);
    uint32_t size = sizeof(req->head) + sizeof(req->input_regs);
    start_resp(modbus, size);
}

void modbus_resp_working(struct modbus *modbus, uint32_t size)
{
    struct modbus_resp *resp = &modbus->resp;
    struct modbus_out *out = &modbus->out;
    if (size == 0) {
        out->status = MODBUS_ERR_TIMEOUT;
    } else if (CRC16_calc(&modbus->resp, size) != 0) {
        out->status = MODBUS_ERR_CRC;
    } else if (resp->head.func & 0x80) {
        out->status = resp->err;
    } else {
        switch (resp->head.func) {
        case FUNC_WRITE_SINGLE_COIL:
            break;
        case FUNC_WRITE_SINGLE_REG:
            break;
        case FUNC_WRITE_MULTIPLE_REGS:
            break;
        case FUNC_READ_INPUT_REGS:
        case FUNC_READ_HOLDING_REGS: {
            uint32_t quality = resp->holding_regs.byte_count * 2;
            out->quality = quality;
            for (uint32_t i = 0; i < quality; i++) {
                out->regs[i] = __REVSH(resp->holding_regs.regs[i]);
            }
        } break;
        default:
            break;
        }
        out->status = 0;
    }

    modbus->resp_callback(out);
}
