#include "MCP4921.h"


void MCP4921_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_3);

#if(SPI_peripheral == use_SW_SPI)
    SW_SPI_GPIO_init();

#elif(SPI_peripheral == use_HW_SPI)
    HW_SPI_init(0, 2, 0);

#endif
}


void MCP4921_write(u8 cmd, u16 dac)
{
    u8 dummy[2];
    u8 tmp[2] = {0, 0};

    tmp[0] = ((u8)(((dac & 0x0F00) >> 8) | cmd));
    tmp[1] = ((u8)(dac & 0x00FF));

    GPIO_SetBits(GPIOC, GPIO_Pin_3);

#if(SPI_peripheral == use_SW_SPI)
    SW_SPI_transfer(tmp, dummy, 2);

#elif(SPI_peripheral == use_HW_SPI)
    HW_SPI_transfer(tmp, dummy, 2);

#endif

    GPIO_ResetBits(GPIOC, GPIO_Pin_3);
    GPIO_SetBits(GPIOC, GPIO_Pin_3);
}


void MCP4921_write_DAC(u16 value)
{
    MCP4921_write(MCP4921_CMD, value);
}

