#include "debug.h"
#include "MCP4921.h"
#include "I2C_LCD.h"


#define VDD             3300
#define ADC_count_max   1023
#define DAC_count_max   4095
#define LM35_V_per_C    10


u8 adc_conv_complete = RESET;


void LED_GPIO_init(void);
void ADC_init(void);
void load_custom_symbol(void);
void print_symbol(u8 x_pos, u8 y_pos, u8 symbol_index);
void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void ADC1_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
    {
        adc_conv_complete = SET;
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}



void main(void)
{
    char buffer[10];

    u16 mv = 0;
    s16 tmp = 0;
    u16 dac_out = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    LED_GPIO_init();
    ADC_init();
    MCP4921_init();
    LCD_init();
    LCD_clear_home();
    LCD_putstr(0, 0, "Tmp/ C:");
    LCD_putstr(0, 1, "DAC mV:");
    load_custom_symbol();
    print_symbol(4, 0, 0);

    while(1)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);

        if(adc_conv_complete)
        {
            mv = ADC_GetConversionValue(ADC1);
            tmp = ((mv * VDD) / (ADC_count_max  * LM35_V_per_C));
            mv = ((mv * ADC_count_max * LM35_V_per_C) / VDD);;
            dac_out = ((mv * DAC_count_max) / VDD);
            adc_conv_complete = RESET;
            GPIO_SetBits(GPIOD, GPIO_Pin_4);
        }

        sprintf(buffer, "%3d", tmp);
        LCD_putstr(13, 0, buffer);

        sprintf(buffer, "%4d ", mv);
        LCD_putstr(12, 1, buffer);

        MCP4921_write_DAC(dac_out);
        Delay_Ms(400);
    };
}


void LED_GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void ADC_init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_241Cycles);
    ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);

    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

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
