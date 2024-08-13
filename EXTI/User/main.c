#include "debug.h"
#include "I2C_LCD.h"


#define pulse_check_1         13
#define pulse_check_2         26

#define rep_high           30000
#define rep_low            20000
#define one_high            1650
#define one_low             1550
#define zero_high           1100
#define zero_low             900


u8 reception_complete = 0;
u8 frame_cnt = 0;
u16 frames[40];

void GPIO_init(void);
void EXTI0_init(void);
void TIM_init(void);
void decode_SIRC(u8 *dvc, u8 *cmd);
u8 get_bits(u8 start_pos, u8 end_pos);
void erase_frames(void);
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        frames[frame_cnt] = TIM_GetCounter(TIM2);
        frame_cnt += 1;
        GPIO_WriteBit(GPIOD, GPIO_Pin_4, RESET);
        TIM_SetCounter(TIM2, 0x0000);

        if(frame_cnt >= pulse_check_1)
        {
            reception_complete = 1;
        }

        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}



void main(void)
{
    char buffer[6];

    u8 cmd = 0x00;
    u8 dvc = 0x00;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    EXTI0_init();
    TIM_init();

    LCD_init();
    LCD_Clear_Home();

    LCD_putstr(2, 0, "CH32 SIRC IR");
    LCD_putstr(0, 1, "C:");
    LCD_putstr(9, 1, "D:");

    while(1)
    {
        decode_SIRC(&dvc, &cmd);

        sprintf(buffer, "0x%02x", dvc);
        LCD_putstr(3, 1, buffer);

        sprintf(buffer, "0x%02x", cmd);
        LCD_putstr(12, 1, buffer);


        GPIO_WriteBit(GPIOD, GPIO_Pin_4, SET);
        Delay_Ms(100);
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


void EXTI0_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd((RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD), ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



void TIM_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

    TIM_DeInit(TIM2);

    TIM_InternalClockConfig(TIM2);
    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 8;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 60000;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_SetCounter(TIM2, 0x0000);
    TIM_Cmd(TIM2, ENABLE);
}


void decode_SIRC(u8 *dvc, u8 *cmd)
{
    if(reception_complete)
    {
        if((frames[pulse_check_1] >= rep_low) && (frames[pulse_check_1] <= rep_high))
        {
            if((frames[pulse_check_2] >= rep_low) && (frames[pulse_check_2] <= rep_high))
            {
                *cmd = get_bits(7, 1);
                *dvc = get_bits(12, 8);
            }
        }

        erase_frames();
        reception_complete = 0;
    }
}


u8 get_bits(u8 start_pos, u8 end_pos)
{
    u8 i = 0x00;
    u8 value = 0x00;

    i = start_pos;
    while(i >= end_pos)
    {
       value <<= 1;

       if((frames[i] >= one_low) && (frames[i] <= one_high))
       {
           value |= 1;
       }
       else if((frames[i] >= zero_low) && (frames[i] <= zero_high))
       {
           value |= 0;
       }
       else
       {
           value = 0xFF;
       }

       i -= 1;
    }

    return value;
}


void erase_frames(void)
{
     for(frame_cnt = 0; frame_cnt < 40; frame_cnt++)
     {
           frames[frame_cnt] = 0x0000;
     }

     frame_cnt = 0;
}
