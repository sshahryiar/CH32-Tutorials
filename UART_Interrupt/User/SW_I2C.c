#include "SW_I2C.h"


void SW_I2C_GPIO_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    SW_I2C_SDA_OUT_init();
}


void SW_I2C_SDA_OUT_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1 | GPIO_Pin_2);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}



void SW_I2C_SDA_IN_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void SW_I2C_init(void)
{
    SW_I2C_GPIO_init();
    SW_I2C_SDA_OUT_init();
    Delay_Ms(10);
    SDA_HIGH();
    SCL_HIGH();
}


void SW_I2C_start(void)
{
    SW_I2C_SDA_OUT_init();
    SDA_HIGH();
    SCL_HIGH();
    Delay_Us(40);
    SDA_LOW();
    Delay_Us(40);
    SCL_LOW();
}


void SW_I2C_stop(void)
{
    SW_I2C_SDA_OUT_init();
    SDA_LOW();
    SCL_HIGH();
    Delay_Us(40);
    SDA_HIGH();
    SCL_HIGH();
    Delay_Us(40);
}


u8 SW_I2C_read(u8 ack)
{
    u8 i = 8;
    u8 j = 0;

   SW_I2C_SDA_IN_init();

    while(i > 0)
    {
        SCL_LOW();
        Delay_Us(20);
        SCL_HIGH();
        Delay_Us(20);
        j <<= 1;

        if(SDA_IN())
        {
            j++;
        }

        Delay_Us(10);
        i--;
    };

    switch(ack)
    {
        case I2C_ACK:
        {
            SW_I2C_ACK_NACK(I2C_ACK);
            break;
        }
        default:
        {
            SW_I2C_ACK_NACK(I2C_NACK);
            break;
        }
    }

    return j;
}


void SW_I2C_write(u8 value)
{
    u8 i = 8;

    SW_I2C_SDA_OUT_init();
    SCL_LOW();

    while(i > 0)
    {
        if(((value & 0x80) >> 7))
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }

        value <<= 1;
        Delay_Us(20);
        SCL_HIGH();
        Delay_Us(20);
        SCL_LOW();
        Delay_Us(20);
        i--;
    };
}


void SW_I2C_ACK_NACK(u8 mode)
{
    SW_I2C_SDA_OUT_init();
    SCL_LOW();

    switch(mode)
    {
        case I2C_ACK:
        {
            SDA_LOW();
            break;
        }
        default:
        {
            SDA_HIGH();
            break;
        }
    }

    Delay_Us(20);
    SCL_HIGH();
    Delay_Us(20);
    SCL_LOW();
}


u8 SW_I2C_wait_ACK(void)
{
    u16 timeout = 0;

    SW_I2C_SDA_OUT_init();

    SDA_HIGH();
    Delay_Us(10);
    SCL_HIGH();
    Delay_Us(10);

    SW_I2C_SDA_IN_init();

    while(SDA_IN())
    {
        timeout++;

        if(timeout > I2C_timeout)
        {
            SW_I2C_stop();
            return 1;
        }
    };

    SCL_LOW();
    return 0;
}

