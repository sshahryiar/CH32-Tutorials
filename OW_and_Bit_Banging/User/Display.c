#include "Display.h"


static const u8 seg_code[10] = {
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
                                };


void write_74HC595(u8 value)
{
    s8 clks = 0x08;
    u8 tmp = 0x00;

    tmp = value;

    GPIO_WriteBit(GPIOC, GPIO_Pin_3, SET);

    while(clks > 0)
    {
        if(tmp & 0x80)
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOC, GPIO_Pin_6, RESET);
        }

        GPIO_WriteBit(GPIOC, GPIO_Pin_5, SET);
        tmp <<= 1;
        clks -= 1;
        GPIO_WriteBit(GPIOC, GPIO_Pin_5, RESET);

    }

    GPIO_WriteBit(GPIOC, GPIO_Pin_3, RESET);
}


void update_display(u16 value)
{
    u8 c = 0;

    c = (u8)(value % 10);
    write_74HC595(seg_code[c]);

    c = (u8)((value % 100) / 10);
    write_74HC595(seg_code[c] & 0x7F);

    c = (u8)((value % 1000) / 100);
    write_74HC595(seg_code[c]);

    c = (u8)(value / 1000);
    write_74HC595(seg_code[c]);

    write_74HC595(0xFF);
}


void clear_display(void)
{
    s8 i = 0x08;

    while(i > 0)
    {
        write_74HC595(0xFF);
        i -= 1;
    }
}
