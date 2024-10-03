#include "debug.h"


void GPIO_init(void);


void main(void)
{
    u8 i = 0;
    u8 state = 0;
    u8 bit_status = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();

    while(1)
    {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == RESET)
        {
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, SET);
            Delay_Ms(40);
            GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
            GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);

            state++;

            if(state > 4)
            {
                state = 0;
            }
            GPIO_WriteBit(GPIOD, GPIO_Pin_4, RESET);
        }

        switch(state)
        {
            case 1:
            {
                GPIO_WriteBit(GPIOD, GPIO_Pin_2, bit_status);
                GPIO_WriteBit(GPIOD, GPIO_Pin_3, !bit_status);
                Delay_Ms(160);
                bit_status ^= 0x01;
                break;
            }

            case 2:
            {
                GPIO_WriteBit(GPIOD, GPIO_Pin_2, bit_status);
                GPIO_WriteBit(GPIOD, GPIO_Pin_3, bit_status);
                Delay_Ms(160);
                bit_status ^= 0x01;
                break;
            }

            case 3:
            {
                GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
                for(i = 0; i < 3; i++)
                {
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, SET);
                    Delay_Ms(60);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
                    Delay_Ms(60);
                }

                GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
                for(i = 0; i < 3; i++)
                {
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET);
                    Delay_Ms(60);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
                    Delay_Ms(60);
                }
                break;
            }

            case 4:
            {
                for(i = 0; i < 6; i++)
                {
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, SET);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET);
                    Delay_Ms(40);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
                    Delay_Ms(40);
                }
                Delay_Ms(300);

                break;
            }

            default:
            {
                for(i = 0; i < 6; i++)
                {
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, RESET);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET);
                    Delay_Ms(60);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_2, SET);
                    GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET);
                    Delay_Ms(60);
                }
                Delay_Ms(260);
                break;
            }
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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
