#include "HMC1022.h"


void cal_LED_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void UART_write(u8 value)
{
    u16 t = HMC1022_timeout;

    USART_SendData(USART1, value);
    while((USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) && (t > 0))
    {
      t--;
      Delay_Ms(1);
    };
}


u16 HMC1022_read_heading(void)
{
    u8 s = 0x00;
    u16 heading = 0x0000;
    u16 t = HMC1022_timeout;
    u8 data_bytes[HMC1022_no_of_data_bytes_returned];

    UART_write(HMC1022_Get_Angular_Measurement);

    for(s = 0; s < HMC1022_no_of_data_bytes_returned; s++)
    {
        data_bytes[s] = USART_ReceiveData(USART1);
        while((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)  && (t > 0))
        {
          t--;
          Delay_Ms(1);
        };
    }

    for(s = 3; s < 8; s++)
    {
        data_bytes[s] -= 0x30;
    }

    heading = ((u16)(data_bytes[3] * 1000));
    heading += ((u16)(data_bytes[4] * 100));
    heading += ((u16)(data_bytes[5] * 10));
    heading += ((u16)(data_bytes[7]));

    if((heading > 0) && (heading < 3599))
    {
        return heading;
    }
}


void HMC1022_calibrate_compass(void)
{
    u8 s = 0x00;

    UART_write(HMC1022_Start_Calibration);

    for(s = 0; s < 60; s++)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(100);
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(900);
    }

    for(s = 0; s < 60; s++)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(400);
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(600);
    }

    UART_write(HMC1022_End_Calibration);
}


void HMC1022_factory_reset()
{
    UART_write(0xA0);
    UART_write(0xAA);
    UART_write(0xA5);
    UART_write(0xC5);
}


void HMC1022_set_I2C_address(u8 i2c_address)
{
    UART_write(0xA0);
    UART_write(0xAA);
    UART_write(0xA5);
    UART_write(i2c_address);
}


void HMC1022_set_declination_angle(u16 angle)
{
    u8 lb = 0;
    u16 hb = 0;

    lb = ((u8)(angle & 0x00FF));

    hb = (angle & 0xFF00);
    hb >>= 8;

    UART_write(HMC1022_Set_Magnetic_Declination_High_Byte);
    UART_write(((u8)hb));

    UART_write(HMC1022_Set_Magnetic_Declination_Low_Byte);
    UART_write(lb);
}


