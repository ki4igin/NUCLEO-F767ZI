
#ifndef __CRC_H__
#define __CRC_H__

#include "stm32f7xx.h"

#define CRC_SIZE 2

void CRC_Init(void);
uint32_t CRC_calc_u16(void *data, uint32_t len);
uint32_t CRC_calc_u8(void *data, uint32_t len);
uint32_t CRC_calc(void *data);
void CRC_to_end_array_u8(void *data, uint32_t len);
void CRC_to_end_array_u16(void *data, uint32_t len);

#endif
