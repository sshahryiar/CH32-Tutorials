#include "HW_SPI.h"


void HW_SPI_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO), ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
}


void HW_SPI_init(u8 mode, u16 clk_div, u8 MSB_first)
{
    SPI_InitTypeDef SPI_InitStructure = {0};

    HW_SPI_GPIO_init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_I2S_DeInit(SPI1);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;

    switch(mode)
    {
        case 1:
        {
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
            break;
        }
        case 2:
        {
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
            break;
        }
        case 3:
        {
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
            break;
        }
        default:
        {
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
            break;
        }
    }

    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    switch(clk_div)
    {
        case 4:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
            break;
        }
        case 8:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
            break;
        }
        case 16:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
            break;
        }
        case 32:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
            break;
        }
        case 64:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
            break;
        }
        case 128:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
            break;
        }
        case 256:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
            break;
        }
        default:
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
            break;
        }
    }

    switch(MSB_first)
    {
        case 1:
        {
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
            break;
        }
        default:
        {
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
            break;
        }
    }

    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}


void HW_SPI_transfer(u8 *tx_bytes, u8 *rx_bytes, u16 packet_size)
{
    u16 i = 0;

    GPIO_ResetBits(GPIOC, GPIO_Pin_4);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));

    for(i = 0; i < packet_size; i++)
    {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI1, tx_bytes[i]);

        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        rx_bytes[i] = SPI_I2S_ReceiveData(SPI1);
    }

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
}
