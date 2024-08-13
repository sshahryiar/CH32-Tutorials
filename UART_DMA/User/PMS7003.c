#include "PMS7003.h"


extern u8 Rx_Buffer[PMS7003_Rx_Buffer];



void USART_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1), ENABLE);

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
    USART_Cmd(USART1, ENABLE);
}


void DMA_init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Rx_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = PMS7003_Rx_Buffer;

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel5, ENABLE);
}


u16 make_word(u8 HB, u8 LB)
{
    u16 temp = 0x0000;

    temp = ((u16)HB);
    temp <<= 8;
    temp |= ((u16)LB);

    return temp;
}


void PMS7003_init(void)
{
    memset(Rx_Buffer, 0x00, sizeof(PMS7003_Rx_Buffer));

    USART_init();
    DMA_init();
    Delay_Ms(10);

    PMS7003_active_mode_read();
    Delay_Ms(100);

    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    Delay_Ms(100);
}


void PMS7003_send_command(u8 cmd, u8 dat)
{
    u8 temp[0x07] = {0};
    u8 i = 0x00;
    u16 LRC = 0x0000;


    memset(temp, 0x00, sizeof(temp));

    temp[0] = PMS7003_start_byte_1;
    temp[1] = PMS7003_start_byte_2;
    temp[2] = cmd;
    temp[3] = 0x00;
    temp[4] = dat;

    for(i = 0; i <= 4; i++)
    {
        LRC += ((u16)temp[i]);
    }

    temp[6] = ((u8)(LRC & 0x00FF));
    temp[5] = ((u8)((LRC & 0xFF00) >> 8));

    for(i = 0; i < 7; i++)
    {
        USART_SendData(USART1, temp[i]);
        while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    }

    Delay_Ms(10);
}


void PMS7003_passive_mode_read(void)
{
    PMS7003_send_command(PMS7003_change_mode_cmd, PMS7003_passive_mode);
}


void PMS7003_active_mode_read(void)
{
    PMS7003_send_command(PMS7003_change_mode_cmd, PMS7003_active_mode);
}


void PMS7003_sleep(void)
{
    PMS7003_send_command(PMS7003_sleep_wakeup_cmd, PMS7003_sleep_mode);
}


void PMS7003_wakeup(void)
{
    PMS7003_send_command(PMS7003_sleep_wakeup_cmd, PMS7003_wakeup_mode);
}


u8 PMS7003_read_raw(void)
{
    u8 i = 0x00;
    u8 status = 0;
    u8 LRCL = 0x00;
    u8 LRCH = 0x00;
    u16 LRC = 0x0000;

    if(DMA_GetFlagStatus(DMA1_FLAG_TC5))
    {
        USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC5);

        if((Rx_Buffer[0] == PMS7003_start_byte_1) && (Rx_Buffer[1] == PMS7003_start_byte_2))
        {
            if((Rx_Buffer[2] == 0x00) && (Rx_Buffer[3] == 0x1C))
            {
                for(i = 0; i < (PMS7003_Rx_Buffer - 2); i++)
                {
                    LRC += ((u16)Rx_Buffer[i]);
                }

                LRCL = ((u8)(LRC & 0x00FF));
                LRCH = ((u8)((LRC & 0xFF00) >> 8));

                if((Rx_Buffer[30] == LRCH) && (Rx_Buffer[31] == LRCL))
                {
                    status = PMS7003_okay;
                }

                else
                {
                    status =  PMS7003_CRC_error;
                }
            }
        }

        else
        {
            status = PMS7003_frame_error;
        }
    }

    else
    {
        status = PMS7003_data_missing;
    }

    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    return status;
}


u8 PMS7003_read_sensor(u8 sector, u16 *data_1, u16 *data_2, u16 *data_3)
{
    u8 status = 0;

    status = PMS7003_read_raw();

    if(status == 0)
    {
        switch(sector)
        {
            case 1:
            {
                *data_1 = make_word(Rx_Buffer[10], Rx_Buffer[11]);
                *data_2 = make_word(Rx_Buffer[12], Rx_Buffer[13]);
                *data_3 = make_word(Rx_Buffer[14], Rx_Buffer[15]);
                break;
            }

            case 2:
            {
                *data_1 = make_word(Rx_Buffer[16], Rx_Buffer[17]);
                *data_2 = make_word(Rx_Buffer[18], Rx_Buffer[19]);
                *data_3 = make_word(Rx_Buffer[20], Rx_Buffer[21]);
                break;
            }

            case 3:
            {
                *data_1 = make_word(Rx_Buffer[22], Rx_Buffer[23]);
                *data_2 = make_word(Rx_Buffer[24], Rx_Buffer[25]);
                *data_3 = make_word(Rx_Buffer[26], Rx_Buffer[27]);
                break;
            }

            default:
            {
                *data_1 = make_word(Rx_Buffer[4], Rx_Buffer[5]);
                *data_2 = make_word(Rx_Buffer[6], Rx_Buffer[7]);
                *data_3 = make_word(Rx_Buffer[8], Rx_Buffer[9]);
                break;
            }
        }
    }

    return status;
}
