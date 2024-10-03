#include "debug.h"
#include "I2C_LCD.h"
#include "string.h"


#define no_of_samples     16


u16 ADC_buffer[no_of_samples];


void ADC_init(void);
void DMA_init(u32 hw_addr, u32 mem_addr);


void main(void)
{
    char buffer[16];

    u16 i = 0;
    u16 avg = 0;
    u16 lx = 0;
    u16 sample = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    ADC_init();
    DMA_init((u32)&ADC1->RDATAR, (u32)ADC_buffer);
    Delay_Init();
    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(1, 0, "CH32V003 ADC + DMA");

    while(1)
    {
        avg = 0;
        memset(ADC_buffer, 0x0000, no_of_samples);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_241Cycles);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        Delay_Ms(50);
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);

        for(i = 0; i < no_of_samples; i++)
        {
            sample = ADC_buffer[i];
            avg += sample;

            sprintf(buffer, "Sample Count: %4u ", i);
            LCD_putstr(0, 1, buffer);

            sprintf(buffer, "Sample Value: %4u ", sample);
            LCD_putstr(0, 2, buffer);

            Delay_Ms(600);
        }

        avg >>= 4;

        lx = (avg * 10);

        sprintf(buffer, "Lux Value/lx: %5u ", lx);
        LCD_putstr(0, 3, buffer);

        Delay_Ms(400);
    }
}


void ADC_init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_75PERCENT);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}


void DMA_init(u32 hw_addr, u32 mem_addr)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = hw_addr;
    DMA_InitStructure.DMA_MemoryBaseAddr = mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = no_of_samples;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}
