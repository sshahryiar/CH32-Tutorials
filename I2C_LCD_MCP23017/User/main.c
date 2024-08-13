#include "debug.h"
#include "I2C_LCD.h"



void main(void)
{
    char buffer[10];
    s16 i = -99;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(1, 0, "WCH32 CH32V003");

    while(1)
    {
        sprintf(buffer, "%d ", i);
        i++;
        LCD_putstr(6, 1, buffer);
        Delay_Ms(400);
    }
}
