#include "crc16.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_ll_bus.h"

/* CRC init function */
void CRC16_Init(void)
{
    /* Peripheral clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_HALFWORD);
    LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_BIT);
    LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
    LL_CRC_SetPolynomialCoef(CRC, 0x8005);
    LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_16B);
}

uint16_t CRC16_calc(void *data, uint32_t size)
{
    LL_CRC_ResetCRCCalculationUnit(CRC);
    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_WORD);
    uint32_t *p_u32 = data;
    for (uint32_t i = 0; i < size / 4; i++) {
        LL_CRC_FeedData32(CRC, *p_u32++);
    }
    uint32_t mod = size % 4;
    switch (mod) {
    case 0: {
    } break;
    case 1: {
        uint8_t *p_u8 = (uint8_t *)p_u32;
        LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_BYTE);
        LL_CRC_FeedData8(CRC, *p_u8);
    } break;
    case 2: {
        uint16_t *p_u16 = (uint16_t *)p_u32;
        LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_HALFWORD);
        LL_CRC_FeedData16(CRC, *p_u16);
    } break;
    case 3: {
        uint16_t *p_u16 = (uint16_t *)p_u32;
        LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_HALFWORD);
        LL_CRC_FeedData16(CRC, *p_u16++);
        uint8_t *p_u8 = (uint8_t *)p_u16;
        LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_BYTE);
        LL_CRC_FeedData8(CRC, *p_u8);
    } break;
    }

    return LL_CRC_ReadData16(CRC);
}

void CRC16_to_end_array(void *data, uint32_t size)
{
    uint16_t crc = CRC16_calc(data, size);
    uint16_t *end = (uint16_t *)((uint8_t *)data + size);
    *end = crc;
}
