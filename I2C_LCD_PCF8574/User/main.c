#include "debug.h"
#include "I2C_LCD.h"


void TIM_init(u16 period, u16 prescalar);


void main(void)
{
    char buffer[10];
    u16 time = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    TIM_init(65000, 48);
    LCD_init();
    LCD_clear_home();

#if(I2C_peripheral == use_SW_I2C)
    LCD_putstr(3, 0, "SW I2C LCD");

#elif(I2C_peripheral == use_HW_I2C)
    LCD_putstr(3, 0, "HW I2C LCD");

#endif

    while(1)
    {
        TIM_SetCounter(TIM2, 0);
        TIM_Cmd(TIM2, ENABLE);
        sprintf(buffer, "%d ", time);
        LCD_putstr(6, 1, buffer);
        TIM_Cmd(TIM2, DISABLE);
        time = TIM_GetCounter(TIM2);
        Delay_Ms(400);
    }
}


void TIM_init(u16 period, u16 prescalar)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

    TIM_DeInit(TIM2);

    TIM_InternalClockConfig(TIM2);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);

    TIM_TimeBaseInitStruct.TIM_Prescaler = (prescalar - 1);
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = (period - 1);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
}
