#include "debug.h"
#include "string.h"
#include "I2C_LCD.h"
#include "PMS7003.h"


u8 Rx_Buffer[PMS7003_Rx_Buffer];


void GPIO_init(void);


void main(void)
{
    char buffer[10];

    u16 data1 = 0;
    u16 data2 = 0;
    u16 data3 = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    PMS7003_init();
    LCD_init();
    LCD_clear_home();
    LCD_putstr(0, 0, "PMS7003M Sensor Data");
    LCD_putstr(0, 1, "PM1.0 ug/m3:");
    LCD_putstr(0, 2, "PM2.5 ug/m3:");
    LCD_putstr(0, 3, "PM 10 ug/m3:");

    while(1)
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        PMS7003_read_sensor(0, &data1, &data2, &data3);
        Delay_Ms(200);

        sprintf(buffer, "%5d ", data1);
        LCD_putstr(14, 1, buffer);
        sprintf(buffer, "%5d ", data2);
        LCD_putstr(14, 2, buffer);
        sprintf(buffer, "%5d ", data3);
        LCD_putstr(14, 3, buffer);

        GPIO_SetBits(GPIOD, GPIO_Pin_4);
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
