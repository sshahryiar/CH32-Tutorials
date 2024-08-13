#include "debug.h"
#include "I2C_LCD.h"
#include "LM75.h"


void GPIO_init(void);
void load_custom_symbol(void);
void print_symbol(u8 x_pos, u8 y_pos, u8 symbol_index);


void main(void)
{
    char buffer[10];
    float tmp = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    LM75_init();
    LCD_init();
    LCD_clear_home();

    LCD_putstr(1, 0, "LM75A CH32V003");
    LCD_putstr(0, 1, "Tc/ C:");
    load_custom_symbol();
    print_symbol(3, 1, 0);

    while(1)
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(200);
        tmp = LM75_get_temp();
        sprintf(buffer, "%3.3f ", tmp);
        LCD_putstr(10, 1, buffer);

        GPIO_SetBits(GPIOD, GPIO_Pin_4);
        Delay_Ms(400);
    }
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


void load_custom_symbol(void)
{
    u8 s = 0;

    u8 custom_symbol[8] =
    {
        0x00, 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00
    };

    LCD_send(0x40, CMD);

    for(s = 0; s < 8; s++)
    {
        LCD_send(custom_symbol[s], DAT);
    }

    LCD_send(0x80, CMD);
}


void print_symbol(u8 x_pos, u8 y_pos, u8 symbol_index)
{
    LCD_goto(x_pos, y_pos);
    LCD_send(symbol_index, DAT);
}
