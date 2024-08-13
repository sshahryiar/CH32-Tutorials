#include "debug.h"
#include "I2C_LCD.h"


u16 value = 0;


void GPIO_init(void);
void TIM_init(u16 period, u16 prescalar);
void TIM1_CC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void TIM1_CC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {
        TIM_SetCounter(TIM1, 0);
        TIM_ClearITPendingBit(TIM1, (TIM_IT_CC1));
    }

    if(TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
    {
        value = TIM_GetCapture2(TIM1);
        TIM_ClearITPendingBit(TIM1, (TIM_IT_CC2));
    }
}



void main(void)
{
    u32 r = 0;

    char buffer[10];

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    TIM_init(40000, 8);

    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(0, 0, "Range/cm:");

    while(1)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_3);
        Delay_Us(10);
        GPIO_ResetBits(GPIOD, GPIO_Pin_3);

        r = (((float)value) / 58);

        sprintf(buffer, "%4d ", r);
        LCD_putstr(0, 1, buffer);

        Delay_Ms(600);
    }
}


void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
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
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

    TIM_PWMIConfig(TIM1, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM1, (TIM_IT_CC1 | TIM_IT_CC2), ENABLE);

    TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    TIM_Cmd(TIM1, ENABLE);
}
