#include "debug.h"
#include "string.h"
#include "I2C_LCD.h"


#define TWTOF240UI_Header_Frame_1            0x55
#define TWTOF240UI_Header_Frame_2            0xAA
#define TWTOF240UI_Tail_Frame                0xFA

#define TWTOF240UI_Measure_CMD               0x81
#define TWTOF240UI_Crosstalk_Cal_CMD         0x82
#define TWTOF240UI_Compensation_CMD          0x83
#define TWTOF240UI_Factory_Reset_CMD         0x84
#define TWTOF240UI_Remaining_Cache_Data_CMD  0x85
#define TWTOF240UI_Peripheral_Sel_CMD        0x86
#define TWTOF240UI_Debug_Seg_1_CMD           0x8A
#define TWTOF240UI_Debug_Seg_2_CMD           0x8B
#define TWTOF240UI_Threshold_Set_CMD         0x91
#define TWTOF240UI_Output_Mode_CMD           0x92


#define RX_Size     16

u8 cnt = 0;
u8 received = 0;
u8 Rx_Buffer[RX_Size] = {0};


void GPIO_init(void);
void USART_init(void);
void USART_send(u8 *send_byte, u8 size);
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        Rx_Buffer[cnt] = USART_ReceiveData(USART1);
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        cnt += 1;

        if((cnt >= RX_Size) || (Rx_Buffer[cnt] == TWTOF240UI_Tail_Frame))
        {
            received = 1;
        }

        USART_ClearFlag(USART1, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}


void main(void)
{
    char buffer[10];

    s16 range = 0;
    u8 TX_Buffer[16] = {0};

    TX_Buffer[0] = TWTOF240UI_Header_Frame_1;
    TX_Buffer[1] = TWTOF240UI_Header_Frame_2;
    TX_Buffer[2] = TWTOF240UI_Measure_CMD;
    TX_Buffer[3] = 0x01;
    TX_Buffer[4] = 0x00;
    TX_Buffer[5] = TWTOF240UI_Tail_Frame;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    USART_init();
    LCD_init();
    LCD_clear_home();
    LCD_putstr(0, 0, "ToF Range (mm):");

    while(1)
    {
        USART_send(TX_Buffer, 6);

        if(received)
        {
            if((Rx_Buffer[0] == TWTOF240UI_Header_Frame_1) && (Rx_Buffer[1] == TWTOF240UI_Header_Frame_2))
            {
               if((Rx_Buffer[2] == TWTOF240UI_Measure_CMD) && (Rx_Buffer[3] == 0x03))
               {
                   if(Rx_Buffer[7] == TWTOF240UI_Tail_Frame)
                   {
                       range = ((s16)Rx_Buffer[4]);
                       range <<= 8;
                       range |= ((s16)Rx_Buffer[5]);
                   }

                   else
                   {
                       range = -1;
                   }
               }

               else
               {
                   range = -2;
               }
            }

            else
            {
                range = -3;
            }

            sprintf(buffer, "%4d ", range);
            LCD_putstr(0, 1, buffer);

            GPIO_SetBits(GPIOD, GPIO_Pin_4);
            memset(Rx_Buffer, 0x00, sizeof(Rx_Buffer));
            cnt = 0;
            received = 0;
        }

        Delay_Ms(400);
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


void USART_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx);

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);;

    USART_Cmd(USART1, ENABLE);
}


void USART_send(u8 *send_byte, u8 size)
{
    u8 i = 0;

    for(i = 0; i < size;i++)
    {
        USART_SendData(USART1, send_byte[i]);
        while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    };
}
