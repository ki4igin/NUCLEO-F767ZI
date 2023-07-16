#ifndef __USART_H__
#define __USART_H__

#include "stm32f7xx.h"

#define USART_MAX_BUF_SIZE 128

void MX_USART3_UART_Init(void);
void MX_UART4_Init(void);
void MX_UART7_Init(void);

void uart4_send_array_dma(void *buf, uint32_t size);
void uart4_receive_array_dma(void *buf, uint32_t size);
void uart4_receive_callback(void *buf, uint32_t size);
void uart7_send_array_dma(void *buf, uint32_t size);

#endif

