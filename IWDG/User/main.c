#include "debug.h"


void GPIO_init(void);
void IWDG_init(void);


void main(void)
{
    u8 i = 0;
    u8 toggle = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    GPIO_init();
    Delay_Init();

    for(i = 0; i < 6; i++)
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, RESET);
        Delay_Ms(20);
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, SET);
        Delay_Ms(120);
    }
    Delay_Ms(1000);

    IWDG_init();

    while(1)
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, toggle);
        toggle = ~toggle;
        IWDG_ReloadCounter();
        Delay_Ms(400);

        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == RESET)
        {
            IWDG_ReloadCounter();

            for(i = 0; i <= 9; i++)
            {
                GPIO_WriteBit(GPIOD, GPIO_Pin_4, toggle);
                Delay_Ms(100);
                toggle = ~toggle;
            }
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, RESET);
            Delay_Ms(2000);
            while(1);
        }

    }
}


void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void IWDG_init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_128);
    IWDG_SetReload(6000);
    IWDG_ReloadCounter();
    IWDG_Enable();
}
