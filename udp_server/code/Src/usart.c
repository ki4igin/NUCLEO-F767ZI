#include "usart.h"
#include "main.h"
#include "tools.h"

#define UART_RECV_TIMEOUT 100

/* USART3 init function */
void MX_USART3_UART_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    /**USART3 GPIO Configuration
    PD8   ------> USART3_TX
    PD9   ------> USART3_RX
    */
    GPIO_InitStruct.Pin = STLK_RX_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(STLK_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = STLK_TX_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(STLK_TX_GPIO_Port, &GPIO_InitStruct);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART3, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART3);
    LL_USART_Enable(USART3);
}

/* UART4 init function */
void MX_UART4_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetUARTClockSource(LL_RCC_UART4_CLKSOURCE_PCLK1);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    /**UART4 GPIO Configuration
    PB14   ------> UART4_DE
    PA11   ------> UART4_RX
    PA12   ------> UART4_TX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = GPIO_AF6_UART4;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = GPIO_AF6_UART4;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 DMA Init */

    /* UART4_RX Init */
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_2, LL_DMA_CHANNEL_4);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_2,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_LOW
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);

    /* UART4_TX Init */
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_4);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_4,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_LOW
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);

    /* UART4 interrupt Init */
    NVIC_SetPriority(UART4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(UART4_IRQn);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_EVEN;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(UART4, &USART_InitStruct);
    LL_USART_EnableDEMode(UART4);
    LL_USART_SetDESignalPolarity(UART4, LL_USART_DE_POLARITY_HIGH);
    LL_USART_SetDEAssertionTime(UART4, 0);
    LL_USART_SetDEDeassertionTime(UART4, 0);
    LL_USART_ConfigAsyncMode(UART4);

    LL_USART_SetRxTimeout(UART4, 35);
    LL_USART_EnableRxTimeout(UART4);
    LL_USART_EnableIT_RTO(UART4);

    LL_USART_EnableDMAReq_TX(UART4);
    LL_USART_EnableDMAReq_RX(UART4);
    // LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);

    LL_USART_Enable(UART4);
}

/* UART7 init function */
void MX_UART7_Init(void)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_RCC_SetUARTClockSource(LL_RCC_UART7_CLKSOURCE_PCLK1);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART7);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
    /**UART7 GPIO Configuration
    PE7   ------> UART7_RX
    PE8   ------> UART7_TX
    PE9   ------> UART7_DE
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* UART7 DMA Init */

    /* UART7_RX Init */
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_5);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_3,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_LOW
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);

    /* UART7_TX Init */
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_1, LL_DMA_CHANNEL_5);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_1,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_LOW
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);

    /* UART7 interrupt Init */
    NVIC_SetPriority(UART7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(UART7_IRQn);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_EVEN;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(UART7, &USART_InitStruct);
    LL_USART_EnableDEMode(UART7);
    LL_USART_SetDESignalPolarity(UART7, LL_USART_DE_POLARITY_HIGH);
    LL_USART_SetDEAssertionTime(UART7, 0);
    LL_USART_SetDEDeassertionTime(UART7, 0);
    LL_USART_ConfigAsyncMode(UART7);

    LL_USART_SetRxTimeout(UART7, 35);
    LL_USART_EnableRxTimeout(UART7);
    LL_USART_EnableIT_RTO(UART7);

    LL_USART_EnableDMAReq_TX(UART7);
    LL_USART_EnableDMAReq_RX(UART7);

    LL_USART_Enable(UART7);
}

void uart4_send_array_dma(void *buf, uint32_t size)
{
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_4,
        (uint32_t)buf,
        LL_USART_DMA_GetRegAddr(UART4, LL_USART_DMA_REG_DATA_TRANSMIT),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_4));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, size);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);
}

static uint32_t uart4_recv_size;
static uint32_t uart4_recv_timeout;

void uart4_receive_array_dma(void *buf, uint32_t size)
{
    uart4_recv_size = size;
    uart4_recv_timeout = UART_RECV_TIMEOUT;
    LL_DMA_ClearFlag_TC2(DMA1);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_2,
        LL_USART_DMA_GetRegAddr(UART4, LL_USART_DMA_REG_DATA_RECEIVE),
        (uint32_t)buf,
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_2));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_2, size);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);
}

void UART4_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RTO(UART4)) {
        LL_USART_ClearFlag_RTO(UART4);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
        uart4_recv_callback(
            uart4_recv_size - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_2));
    }
}

void uart7_send_array_dma(void *buf, uint32_t size)
{
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_1,
        (uint32_t)buf,
        LL_USART_DMA_GetRegAddr(UART7, LL_USART_DMA_REG_DATA_TRANSMIT),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_1));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, size);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}

static uint32_t uart7_recv_size;
static uint32_t uart7_recv_timeout;

void uart7_recv_array_dma(void *buf, uint32_t size)
{
    uart7_recv_size = size;
    uart7_recv_timeout = UART_RECV_TIMEOUT;
    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_3,
        LL_USART_DMA_GetRegAddr(UART7, LL_USART_DMA_REG_DATA_RECEIVE),
        (uint32_t)buf,
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_3));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, size);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_3);
}

void UART7_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RTO(UART7)) {
        LL_USART_ClearFlag_RTO(UART7);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
        uart7_recv_callback(
            uart7_recv_size - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_3));
    }
}

void SysTick_Callback(void)
{
    if (LL_DMA_IsEnabledStream(DMA1, LL_DMA_STREAM_2)) {
        if (--uart4_recv_timeout == 0) {
            LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
            uart4_recv_callback(
                uart4_recv_size - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_2));
        }
    }
    if (LL_DMA_IsEnabledStream(DMA1, LL_DMA_STREAM_3)) {
        if (--uart7_recv_timeout == 0) {
            LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
            uart7_recv_callback(
                uart7_recv_size - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_3));
        }
    }
}
