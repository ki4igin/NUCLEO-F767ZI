
#ifndef __CRC16_H__
#define __CRC16_H__

#include "stm32f7xx.h"

#define CRC16_SIZE 2

void crc16_init(void);
uint16_t crc16_calc(void *data, uint32_t size);
void crc16_to_end_array(void *data, uint32_t size);

#endif
