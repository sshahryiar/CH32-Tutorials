#include "OW.h"


void OW_IN_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_DeInit(GPIOD);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void OW_OUT_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_DeInit(GPIOD);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


u8 onewire_reset(void)
{
     u8 res = FALSE;

     OW_OUT_init();

     OW_OUT_LOW;
     Delay_Us(480);
     OW_OUT_HIGH;
     Delay_Us(60);

     OW_IN_init();
     res = OW_IN;
     Delay_Us(480);

     return res;
}


void onewire_write_bit(u8 bit_value)
{
    OW_OUT_init();
    OW_OUT_LOW;

    if(bit_value)
    {
        Delay_Us(104);
        OW_OUT_HIGH;
    }
}


u8 onewire_read_bit(void)
{
    OW_OUT_init();
    OW_OUT_LOW;
    OW_OUT_HIGH;
    OW_IN_init();
    Delay_Us(15);

    return(OW_IN);
}


void onewire_write(u8 value)
{
     u8 s = 0x00;

     OW_OUT_init();

     while(s < 8)
     {
          if((value & (1 << s)))
          {
              OW_OUT_LOW;
              __NOP();
              OW_OUT_HIGH;
              Delay_Us(60);
          }

          else
          {
              OW_OUT_LOW;
              Delay_Us(60);
              OW_OUT_HIGH;
              __NOP();
          }

          s++;
     }
}


u8 onewire_read(void)
{
     u8 s = 0x00;
     u8 value = 0x00;

     while(s < 8)
     {
         OW_OUT_init();
          OW_OUT_LOW;
          __NOP();
          OW_OUT_HIGH;
          OW_IN_init();

          if(OW_IN)
          {
              value |=  (1 << s);
          }

          Delay_Us(60);

          s++;
     }

     return value;
}

