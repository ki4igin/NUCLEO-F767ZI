#include "modbus.h"
#include "crc16.h"
#include "assert.h"
#include "debug.h"

enum modbus_func {
    FUNC_READ_COILS = 0x01,
    FUNC_READ_DISCRETE_INPUTS = 0x02,
    FUNC_READ_HOLDING_REGS = 0x03,
    FUNC_READ_INPUT_REGS = 0x04,
    FUNC_WRITE_SINGLE_COIL = 0x05,
    FUNC_WRITE_SINGLE_REG = 0x06,
    FUNC_READ_EXCEPTION_STATUS = 0x07,
    // FUNC_DIAGNOSTIC = 0x08,
    // FUNC_GET_COM_EVENT_COUNTER = 0x0B,
    // FUNC_GET_COM_EVENT_LOG = 0x0C,
    // FUNC_REPORT_SERVER_ID = 0x11,
    // FUNC_READ_DEVICE_ID = 0x2B,
    FUNC_MASK_WRITE_REG = 0x16,
    FUNC_WRITE_MULTIPLE_REGS = 0x10,
    // FUNC_WRITE_MULTIPLE_COILS = 0x0F,
    FUNC_READ_WRITE_MULTIPLE_REGS = 0x17,
    // FUNC_READ_FIFO = 0x18,
};

void modbus_init(void)
{
    crc16_init();
}

static void start_resp(struct modbus *modbus, uint32_t size)
{
    crc16_to_end_array(&modbus->req, size);
    modbus->send_req(&modbus->req, size + CRC16_SIZE);
    modbus->recv_resp(&modbus->resp, sizeof(modbus->resp));
}

void modbus_read_coils(struct modbus *modbus, uint16_t addr, uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_COILS;
    req->r_coils.addr = __REVSH(addr);
    req->r_coils.quality = __REVSH(quality);
    uint32_t size = sizeof(req->head) + sizeof(req->r_coils);
    start_resp(modbus, size);
}

void modbus_write_single_coil(
    struct modbus *modbus,
    uint16_t addr,
    enum modbus_coil_state state)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_SINGLE_COIL;
    req->coil.addr = __REVSH(addr);
    req->coil.state = __REVSH(state);
    uint32_t size = sizeof(req->head) + sizeof(req->coil);
    start_resp(modbus, size);
}

// void modbus_write_multi_coils(
//     struct modbus *modbus,
//     uint16_t addr,
//     uint8_t vals)
// {
//     struct modbus_req *req = &modbus->req;
//     req->head.func = FUNC_WRITE_MULTIPLE_COILS;
//     req->mask_reg.addr = __REVSH(addr);
//     req->mask_reg.and_mask = __REVSH(and_mask);
//     req->mask_reg.or_mask = __REVSH(or_mask);
//     uint32_t size = sizeof(req->head) + sizeof(req->mask_reg);
//     start_resp(modbus, size);
// }

void modbus_write_single_reg(struct modbus *modbus, uint16_t addr, uint16_t val)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_SINGLE_REG;
    req->reg.addr = __REVSH(addr);
    req->reg.val = __REVSH(val);
    uint32_t size = sizeof(req->head) + sizeof(req->reg);
    start_resp(modbus, size);
}

void modbus_write_mask_reg(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t and_mask,
    uint16_t or_mask)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_MASK_WRITE_REG;
    req->mask_reg.addr = __REVSH(addr);
    req->mask_reg.and_mask = __REVSH(and_mask);
    req->mask_reg.or_mask = __REVSH(or_mask);
    uint32_t size = sizeof(req->head) + sizeof(req->mask_reg);
    start_resp(modbus, size);
}

void modbus_write_single_reg32(
    struct modbus *modbus,
    uint16_t addr,
    uint32_t val)
{
    uint32_t rev_word = val << 16 | val >> 16;
    modbus_write_multi_regs(modbus, addr, (uint16_t *)&rev_word, 2);
}

void modbus_write_multi_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t *regs,
    uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_WRITE_MULTIPLE_REGS;
    req->multi_regs.addr = __REVSH(addr);
    req->multi_regs.quality = __REVSH(quality);
    req->multi_regs.byte_count = quality * 2;
    uint32_t size = sizeof(req->head) + sizeof(req->multi_regs)
                  - REGS_COUNT_MAX * 2 + quality * 2;
    uint16_t *p = req->multi_regs.vals;
    do {
        *p++ = __REVSH(*regs++);
    } while (--quality);
    start_resp(modbus, size);
}

void modbus_read_holding_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_HOLDING_REGS;
    req->holding_regs.addr = __REVSH(addr);
    req->holding_regs.quality = __REVSH(quality);
    uint32_t size = sizeof(req->head) + sizeof(req->holding_regs);
    start_resp(modbus, size);
}

void modbus_read_exception_status(struct modbus *modbus)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_EXCEPTION_STATUS;
    uint32_t size = sizeof(req->head);
    start_resp(modbus, size);
}

void modbus_read_input_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_INPUT_REGS;
    req->input_regs.addr = __REVSH(addr);
    req->input_regs.quality = __REVSH(quality);
    uint32_t size = sizeof(req->head) + sizeof(req->input_regs);
    start_resp(modbus, size);
}

void modbus_read_write_multiple_regs(
    struct modbus *modbus,
    uint16_t read_addr,
    uint16_t read_quality,
    uint16_t write_addr,
    uint16_t *write_regs,
    uint16_t write_quality)
{
    struct modbus_req *req = &modbus->req;
    req->head.func = FUNC_READ_WRITE_MULTIPLE_REGS;
    req->rw_multi_regs.read_addr = __REVSH(read_addr);
    req->rw_multi_regs.read_quality = __REVSH(read_quality);
    req->rw_multi_regs.write_addr = __REVSH(write_addr);
    req->rw_multi_regs.write_quality = __REVSH(write_quality);
    req->rw_multi_regs.write_byte_count = write_quality * 2;
    uint32_t size = sizeof(req->head) + sizeof(req->rw_multi_regs)
                  - REGS_COUNT_MAX * 2 + write_quality * 2;
    uint16_t *p = req->rw_multi_regs.write_vals;
    do {
        *p++ = __REVSH(*write_regs++);
    } while (--write_quality);
    start_resp(modbus, size);
}

void modbus_resp_working(struct modbus *modbus, uint32_t size)
{
    struct modbus_resp *resp = &modbus->resp;
    struct modbus_out *out = &modbus->out;
    if (size == 0) {
        out->status = MODBUS_ERR_TIMEOUT;
    } else if (crc16_calc(&modbus->resp, size) != 0) {
        out->status = MODBUS_ERR_CRC;
    } else if (resp->head.func & 0x80) {
        out->status = resp->err;
    } else {
        switch (resp->head.func) {
        case FUNC_READ_COILS: {
            uint32_t quality = resp->r_coils.byte_count;
            out->quality = quality;
            for (uint32_t i = 0; i < quality; i++) {
                out->coils[i] = resp->r_coils.vals[i];
            }
        } break;
        case FUNC_WRITE_SINGLE_COIL: {
        } break;
        case FUNC_WRITE_SINGLE_REG: {
        } break;
        case FUNC_WRITE_MULTIPLE_REGS: {
        } break;
        case FUNC_MASK_WRITE_REG: {
        } break;
        case FUNC_READ_EXCEPTION_STATUS: {
            out->exc_status = resp->exc_status;
        } break;
        case FUNC_READ_WRITE_MULTIPLE_REGS:
        case FUNC_READ_INPUT_REGS:
        case FUNC_READ_HOLDING_REGS: {
            uint32_t quality = resp->read_regs.byte_count * 2;
            out->quality = quality;
            for (uint32_t i = 0; i < quality; i++) {
                out->regs[i] = __REVSH(resp->read_regs.vals[i]);
            }
        } break;
        default: {
        } break;
        }
        out->status = 0;
    }
    modbus->resp_callback(out);
}
