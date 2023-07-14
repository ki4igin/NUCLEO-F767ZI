
#ifndef __CRC_H__
#define __CRC_H__

#include "stm32f7xx.h"

void CRC_Init(void);
uint32_t CRC_is_valid_u8(uint8_t *data, uint32_t len);
uint32_t CRC_is_valid_u16(uint16_t *data, uint32_t len);
uint32_t CRC_calc_u16(void *data, uint32_t len);
uint32_t CRC_calc_u8(void *data, uint32_t len);
uint32_t CRC_calc(void *data);

#endif
