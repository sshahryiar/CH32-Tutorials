#include "debug.h"


#define WWDG_CNT        0x7F


void GPIO_init(void);
void WWDG_init(void);
static void WWDG_NVIC_Config(void);


void main(void)
{
    u8 i = 0;
    u8 toggle = 0;
    u8 wwdg_tr = 0;
    u8 wwdg_wr = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    GPIO_init();
    Delay_Init();

    for(i = 0; i < 6; i++)
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET);
        Delay_Ms(20);
        GPIO_WriteBit(GPIOD, GPIO_Pin_2, SET);
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
        Delay_Ms(120);
    }
    Delay_Ms(1000);

    WWDG_init();
    wwdg_wr = WWDG->CFGR & 0x7F;

    while(1)
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_2, toggle);
        toggle = ~toggle;

        wwdg_tr = WWDG->CTLR & 0x7F;
        if(wwdg_tr < wwdg_wr)
        {
            WWDG_SetCounter(WWDG_CNT);;
        }

        Delay_Ms(40);

        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == RESET)
        {
            for(i = 0; i < 2; i++)
            {
                GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET);
                Delay_Ms(2);
                GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
                Delay_Ms(6);
            }
            WWDG_SetCounter(0x00);
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

    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void WWDG_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    WWDG_SetCounter(WWDG_CNT);
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(0x5F);
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Config();
    WWDG_EnableIT();
}


static void WWDG_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
