#include "Display.h"


static const u8 seg_pos[4] = {
                                   0x01, // 1st
                                   0x02, // 2nd
                                   0x04, // 3rd
                                   0x08, // 4th
                             };


static const u8 seg_code[16] = {
                                   0xC0, // 0
                                   0xF9, // 1
                                   0xA4, // 2
                                   0xB0, // 3
                                   0x99, // 4
                                   0x92, // 5
                                   0x82, // 6
                                   0xF8, // 7
                                   0x80, // 8
                                   0x90, // 9
                                   0x88, // A
                                   0x83, // b
                                   0xC6, // C
                                   0xA1, // d
                                   0x86, // E
                                   0x8E, // F
                                };


void SPI_send_value(u16 value)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, SET);

    SPI_I2S_SendData(SPI1, value);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    GPIO_WriteBit(GPIOC, GPIO_Pin_3, RESET);
}


void update_display(u8 value, u8 pos, u8 dot)
{
    u16 temp = 0x0000;

    temp = ((u16)seg_code[value]);

    if(dot)
    {
        temp &= 0x7F;
    }

    temp <<= 0x08;
    temp |= ((u16)seg_pos[pos]);

    SPI_send_value(temp);
}


void clear_display(void)
{
    u8 i = 0x00;

    for(i = 0; i < 4; i++)
    {
        update_display(0xFF, i, 0x00);
    }
}
