
#ifndef __MODBUS_H
#define __MODBUS_H

#include "stm32f7xx.h"

#define REGS_COUNT_MAX 4

enum modbus_coil_state {
    COIL_OFF = 0x0000,
    COIL_ON = 0xFF00,
};

enum modbus_err {
    MODBUS_ERR_ILLEGAL_FUNCTION = 0x01,
    MODBUS_ERR_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_ERR_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_ERR_SERVER_DEVICE_FAILURE = 0x04,
    MODBUS_ERR_ACKNOWLEDGE = 0x05,
    MODBUS_ERR_SERVER_DEVICE_BUSY = 0x06,
    MODBUS_ERR_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_ERR_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_ERR_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,
    MODBUS_ERR_TIMEOUT = 0x10,
    MODBUS_ERR_CRC = 0x11,
} __attribute__((__packed__));

struct modbus_out {
    enum modbus_err status;
    uint16_t quality;

    union {
        uint8_t exc_status;
        uint16_t regs[REGS_COUNT_MAX];
        uint8_t coils[REGS_COUNT_MAX * 2];
    };
};

struct modbus_head {
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

struct modbus_mask_reg {
    uint16_t addr;
    uint16_t and_mask;
    uint16_t or_mask;
};

struct modbus_req {
    struct modbus_head head;

    union {
        struct modbus_coil coil;

        struct {
            uint16_t addr;
            uint16_t quality;
        } coils;

        struct modbus_reg reg;

        struct modbus_mask_reg mask_reg;

        struct {
            uint16_t addr;
            uint16_t quality;
            uint8_t byte_count;
            uint16_t vals[REGS_COUNT_MAX];
        } __attribute__((packed)) multi_regs;

        struct {
            uint16_t read_addr;
            uint16_t read_quality;
            uint16_t write_addr;
            uint16_t write_vals[REGS_COUNT_MAX];
            uint16_t write_quality;
            uint8_t write_byte_count;
        } __attribute__((packed)) rw_multi_regs;

        struct {
            uint16_t addr;
            uint16_t quality;
        } holding_regs, input_regs;
    };
};

struct modbus_resp {
    struct modbus_head head;

    union {
        struct {
            uint8_t byte_count;
            uint16_t vals[REGS_COUNT_MAX];
        } __attribute__((packed)) read_regs;

        struct modbus_coil coil;

        struct {
            uint8_t byte_count;
            uint8_t vals[REGS_COUNT_MAX * 2];
        } __attribute__((packed)) coils;

        struct modbus_reg reg;

        struct modbus_mask_reg mask_reg;

        struct {
            uint16_t addr;
            uint16_t quality;
        } multi_regs;

        enum modbus_err err;
        uint8_t exc_status;
    };
} __attribute__((packed));

typedef void (*modbus_send_fn)(void *data, uint32_t size);
typedef void (*modbus_recv_fn)(void *data, uint32_t size);
typedef void (*modbus_callback_fn)(struct modbus_out *out);

struct modbus {
    struct modbus_out out;
    struct modbus_req req;
    struct modbus_resp resp;
    modbus_send_fn send_req;
    modbus_recv_fn recv_resp;
    modbus_callback_fn resp_callback;
};

void modbus_init(void);
void modbus_resp_working(struct modbus *modbus, uint32_t size);
void modbus_write_single_coil(
    struct modbus *modbus,
    uint16_t addr,
    enum modbus_coil_state state);
void modbus_write_single_reg(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t val);
void modbus_write_multi_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t *data,
    uint16_t quality);
void modbus_read_holding_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t quality);
void modbus_read_input_regs(
    struct modbus *modbus,
    uint16_t addr,
    uint16_t quality);

#endif
