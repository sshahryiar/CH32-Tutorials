#include "debug.h"
#include "I2C_LCD.h"


void GPIO_init(void);
void TIM_init(u16 period, u16 prescalar);


void main(void)
{
    char buffer[16];

    u16 t = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    GPIO_init();
    TIM_init(4800, 10000);
    Delay_Init();

    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(1, 0, "WCH32 CH32V003");

    while(1)
    {
        sprintf(buffer, "%u ", t);
        LCD_putstr(6, 1, buffer);
        t = TIM_GetCounter(TIM2);
        if(t > 2400)
        {
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, SET);
        }
        else
        {
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, RESET);
        }
    }
}


void GPIO_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void TIM_init(u16 period, u16 prescalar)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

    TIM_DeInit(TIM2);

    TIM_InternalClockConfig(TIM2);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);

    TIM_TimeBaseInitStruct.TIM_Prescaler = (prescalar - 1);                     // 48MHz/10000 = 4800Hz = 208.33us Tick
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = (period - 1);                           // Timer max count. Therefore, overflow occurs every 4800 * 208.33us = 1s
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM2, ENABLE);
}
