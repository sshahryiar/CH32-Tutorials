#include "debug.h"
#include "I2C_LCD.h"


u16 value = 0;


void TIM_init(u16 period, u16 prescalar);
void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void TIM1_CC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {
        value = TIM_GetCapture1(TIM1);
        TIM_SetCounter(TIM1, 0);
    }

    TIM_ClearITPendingBit( TIM1, TIM_IT_CC1);
}



void main(void)
{
    u32 f = 0;

    char buffer[10];

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    TIM_init(10000, 48);

    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(0, 0, "Freq./Hz:");

    while(1)
    {
        f = (1000000 / (float)value);

        sprintf(buffer, "%7d ", f);
        LCD_putstr(0, 1, buffer);

        Delay_Ms(600);
    }
}


void TIM_init(u16 period, u16 prescalar)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_ICInitTypeDef TIM_ICInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);

    TIM_TimeBaseInitStructure.TIM_Period = (period - 1);
    TIM_TimeBaseInitStructure.TIM_Prescaler = (prescalar - 1);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

    TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);

    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    TIM_Cmd(TIM1, ENABLE);
}
