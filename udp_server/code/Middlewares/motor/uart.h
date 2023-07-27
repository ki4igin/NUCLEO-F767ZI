#ifndef __UART_H__
#define __UART_H__

#include "stm32f7xx.h"

#define UART_RECV_TIMEOUT 100

void uart4_init(void);
void uart7_init(void);

void uart4_send_array_dma(void *buf, uint32_t size);
void uart4_recv_array_dma(void *buf, uint32_t size);
void uart4_recv_callback(uint32_t size);

void uart7_send_array_dma(void *buf, uint32_t size);
void uart7_recv_array_dma(void *buf, uint32_t size);
void uart7_recv_callback(uint32_t size);

#endif
