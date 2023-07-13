#include "crc.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_ll_bus.h"

/* CRC init function */
void CRC_Init(void)
{
    /* Peripheral clock enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

    LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_BYTE);
    LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_BIT);
    LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
    LL_CRC_SetPolynomialCoef(CRC, 0x8005);
    LL_CRC_SetPolynomialSize(CRC, LL_CRC_POLYLENGTH_16B);
}

uint32_t CRC_is_valid_u16(uint16_t *data, uint32_t len)
{
    LL_CRC_ResetCRCCalculationUnit(CRC);
    do {
        LL_CRC_FeedData16(CRC, *data++);
    } while (--len);
    return LL_CRC_ReadData16(CRC) == 0;
}

uint32_t CRC_is_valid_u8(uint8_t *data, uint32_t len)
{
    LL_CRC_ResetCRCCalculationUnit(CRC);
    do {
        LL_CRC_FeedData8(CRC, *data++);
    } while (--len);
    return LL_CRC_ReadData16(CRC) == 0;
}
