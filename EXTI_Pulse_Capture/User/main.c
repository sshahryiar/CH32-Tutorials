#include "debug.h"
#include "I2C_LCD.h"


#define DHT11_timeout                 200
#define DHT11_response_max            90
#define DHT11_response_min            80
#define DHT11_logic_high_max          78
#define DHT11_logic_high_min          62
#define DHT11_logic_low_max           30
#define DHT11_logic_low_min           20
#define DHT11_hold_time               18

#define DHT11_no_of_pulses            42

#define DHT11_no_error                -117
#define DHT11_CRC_error               -118
#define DHT11_sync_error              -119
#define DHT11_sensor_error            -120


u8 state = 0x00;
u8 reception_complete = 0x00;
u8 frame_cnt = 0x00;
u16 time_count = 0x0000;
u16 slots[DHT11_no_of_pulses];


void DHT_IN_init(void);
void DHT_OUT_init(void);
void LED_GPIO_init(void);
void EXTI0_Init(void);
void EXTI0_DeInit(void);
void TIM_init(void);
void request_data(void);
void clear_frames(void);
s8 get_data(s8 *p1, s8 *p2);
s8 decode_data(u8 start_pos, u8 end_pos);
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void EXTI7_0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        if(state == 0)
        {
            TIM_SetCounter(TIM1, 0x0000);
        }

        else
        {
            time_count = TIM_GetCounter(TIM1);
            slots[frame_cnt] = time_count;
            frame_cnt += 1;
        }


        if(time_count >= DHT11_timeout)
        {
            frame_cnt = DHT11_no_of_pulses;
        }

        if(frame_cnt >= DHT11_no_of_pulses)
        {
            TIM_Cmd(TIM1, DISABLE);
            EXTI0_DeInit();
            reception_complete = 1;
        }

        state = ~state;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}


void main(void)
{
    char buffer[10];
    s8 T = 0x00;
    s8 RH = 0x00;
    s8 error_code = 0x00;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    LED_GPIO_init();
    Delay_Init();
    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(0, 0, "R.H/%:");
    LCD_putstr(0, 1, "Ta/'C:");
    LCD_putstr(0, 2, "Raw:");

    while(1)
    {
        request_data();

        if(reception_complete)
        {
            error_code = get_data(&RH, &T);

            switch(error_code)
            {
                case DHT11_CRC_error:
                {
                    LCD_putstr(0, 3, "CRC Error!   ");
                    break;
                }
                case DHT11_sync_error:
                {
                    LCD_putstr(0, 3, "Sync Error!  ");
                    break;
                }
                case DHT11_sensor_error:
                {
                    LCD_putstr(0, 3, "Sensor Error!");
                    break;
                }
                default:
                {
                    LCD_putstr(0, 3, "All okay.    ");
                    break;
                }
            }

            if(error_code == DHT11_no_error)
            {
                sprintf(buffer, "%02d", RH);
                LCD_putstr(18, 0, buffer);

                sprintf(buffer, "%02d", T);
                LCD_putstr(18, 1, buffer);
            }
            else
            {
                LCD_putstr(18, 0, "--");
                LCD_putstr(18, 1, "--");
            }

            clear_frames();
            GPIO_SetBits(GPIOD, GPIO_Pin_4);
            reception_complete = 0;
        }

        Delay_Ms(1000);
    }
}


void DHT_IN_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd((RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD), ENABLE);//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
}


void DHT_OUT_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_DeInit(GPIOD);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void LED_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_DeInit(GPIOD);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void EXTI0_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void EXTI0_DeInit(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void TIM_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};

    TIM_DeInit(TIM1);

    TIM_InternalClockConfig(TIM1);
    TIM_SelectInputTrigger(TIM1, TIM_TS_ITR0);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 7;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 64999;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
    TIM_Cmd(TIM1, ENABLE);
}


void request_data(void)
{
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    DHT_OUT_init();
    GPIO_ResetBits(GPIOD, GPIO_Pin_0);
    Delay_Ms(DHT11_hold_time);
    GPIO_SetBits(GPIOD, GPIO_Pin_0);
    DHT_IN_init();
    EXTI0_Init();
    TIM_init();

    if(TIM_GetCounter(TIM1) >= DHT11_timeout)
    {
        frame_cnt = DHT11_no_of_pulses;
    }
}


void clear_frames(void)
{
    u8 i = 0x00;

    for(i = 0; i < DHT11_no_of_pulses; i++)
    {
        slots[i] = 0x00;
    }

    state = 0;
    frame_cnt = 0x00;
    time_count = 0x0000;
    TIM_SetCounter(TIM1, 0x0000);
}


s8 get_data(s8 *p1, s8 *p2)
{
    s8 tmp1 = 0x00;
    s8 tmp2 = 0x00;
    s8 tmp3 = 0x00;
    s8 tmp4 = 0x00;
    s16 CRC = 0x0000;
    s16 temp = 0x0000;

    char buffer[20];

    frame_cnt = 0x0000;

    if(((slots[0]) >= DHT11_response_min) && ((slots[0]) <= DHT11_response_max))
    {
        tmp1 = decode_data(1, 8);
        tmp2 = decode_data(9, 16);
        temp = (tmp1 + tmp2);
        CRC = temp;

        if(temp == DHT11_sync_error)
        {
            return DHT11_sync_error;
        }
        else
        {
            *p1 = tmp1;
        }

        tmp3 = decode_data(17, 24);
        tmp4 = decode_data(25, 32);
        temp = (tmp3 + tmp4);
        CRC += temp;

        if(temp == DHT11_sync_error)
        {
            return DHT11_sync_error;
        }
        else
        {
            *p2 = tmp3;
        }

        temp = decode_data(33, 40);
        temp &= 0xFF;

        sprintf(buffer, "%02X", tmp1);
        LCD_putstr(6, 2, buffer);

        sprintf(buffer, "%02X", tmp2);
        LCD_putstr(9, 2, buffer);

        sprintf(buffer, "%02X", tmp3);
        LCD_putstr(12, 2, buffer);

        sprintf(buffer, "%02X", tmp4);
        LCD_putstr(15, 2, buffer);

        sprintf(buffer, "%02X", temp);
        LCD_putstr(18, 2, buffer);

        if((CRC & 0x00FF) == temp)
        {
            return DHT11_no_error;
        }
        else
        {
            return DHT11_CRC_error;
        }
    }
    else
    {
        return DHT11_sensor_error;
    }
}


s8 decode_data(u8 start_pos, u8 end_pos)
{
    u8 i = 0x00;
    s8 tmp = 0x00;

    for(i = start_pos; i <= end_pos; i++)
    {
        tmp <<= 1;

        if((slots[i] >= DHT11_logic_high_min) && (slots[i] <= DHT11_logic_high_max))
        {
            tmp |= 1;
        }
        else if((slots[i] >= DHT11_logic_low_min) && (slots[i] <= DHT11_logic_low_max))
        {
            ;
        }
        else
        {
            return DHT11_sync_error;
        }
    }

    return tmp;
}
