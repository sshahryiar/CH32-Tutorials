#include "SW_SPI.h"


void SW_SPI_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
}


u8 SW_SPI_send_receive(u8 value)
{
    s8 i = 8;
    u8 retval = 0;

    while(i > 0)
    {
#if(sw_spi_mode == spi_mode_0)
        if(value & 0x80)
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, RESET);
        }

        value <<= 1;
        retval <<= 1;

        GPIO_WriteBit(GPIOC, GPIO_Pin_5, SET);
        retval |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, RESET);

#elif(sw_spi_mode == spi_mode_1)
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, SET);
        if(value & 0x80)
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, RESET);
        }

        value <<= 1;
        retval <<= 1;

        GPIO_WriteBit(GPIOC, GPIO_Pin_5, RESET);
        retval |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);

#elif(sw_spi_mode == spi_mode_2)
        if(value & 0x80)
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, RESET);
        }

        value <<= 1;
        retval <<= 1;

        GPIO_WriteBit(GPIOC, GPIO_Pin_5, RESET);
        retval |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, SET);

#elif(sw_spi_mode == spi_mode_3)
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, RESET);
        if(value & 0x80)
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, RESET);
        }

        value <<= 1;
        retval <<= 1;

        GPIO_WriteBit(GPIOC, GPIO_Pin_5, SET);
        retval |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7);


#endif

        i--;
    }

    return retval;
}



void SW_SPI_transfer(u8 *tx_bytes, u8 *rx_bytes, u16 packet_size)
{
    u16 i = 0;

    GPIO_ResetBits(GPIOC, GPIO_Pin_4);

    for(i = 0; i < packet_size; i++)
    {
        rx_bytes[i] = SW_SPI_send_receive(tx_bytes[i]);
    }

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
}
