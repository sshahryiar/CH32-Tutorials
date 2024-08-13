#include "debug.h"


u8 toggle = 0;
u16 ms = 0;


void GPIO_init(void);
void SysTick_init(u32 counter);
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));



void SysTick_Handler(void)
{
    ms++;

    if(ms > 999)
    {
        ms = 0;
        toggle = ~toggle;
    }

    SysTick->SR = 0;
}


void main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    SysTick_init(8000);

    while(1)
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, toggle);
    };
}


void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void SysTick_init(u32 counter)
{
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->SR &= ~(1 << 0);
    SysTick->CMP = (counter - 1);
    SysTick->CNT = 0;
    SysTick->CTLR = 0x000F;
}



