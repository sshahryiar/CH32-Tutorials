#include "debug.h"
#include "I2C_LCD.h"


#define buffer_size     2

#define LM35_V_per_C    10
#define VDD             3300
#define ADC_count_max   1023
#define ADC_resolution  (VDD/ADC_count_max)


u16 ADC_buffer[buffer_size];


void GPIO_init(void);
void ADC_init(void);
void DMA_init(u32 hw_addr, u32 mem_addr);


void main(void)
{
    char buffer[10];

    char i = 0;

    int16_t avg_1 = 0;
    int16_t avg_2 = 0;

    int32_t t1 = 0;
    int32_t t2 = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1 );
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    ADC_init();
    DMA_init((u32)&ADC1->RDATAR, (u32)ADC_buffer);
    LCD_init();
    LCD_Clear_Home();
    LCD_putstr(0, 0, "T1/'C:");
    LCD_putstr(0, 1, "T2/'C:");

    while(1)
    {
        avg_1 = 0;
        avg_2 = 0;

        for(i = 0; i < 8; i++)
        {
            GPIO_ResetBits(GPIOD, GPIO_Pin_4);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            Delay_Ms(60);
            ADC_SoftwareStartConvCmd(ADC1, DISABLE);

            avg_1 += ADC_buffer[0];
            avg_2 += ADC_buffer[1];

            GPIO_SetBits(GPIOD, GPIO_Pin_4);
            Delay_Ms(60);
        }

        avg_1 >>= 3;
        avg_2 >>= 3;

        t1 = ((float)avg_1 / ((float)ADC_resolution * LM35_V_per_C));
        t2 = ((float)avg_2 / ((float)ADC_resolution) * LM35_V_per_C);

        sprintf(buffer, "%3d", t1);
        LCD_putstr(13, 0, buffer);
        sprintf(buffer, "%3d", t2);
        LCD_putstr(13, 1, buffer);

        Delay_Ms(600);


    }
}


void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void ADC_init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = buffer_size;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_241Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_241Cycles);

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
    DMA_InitStructure.DMA_BufferSize = buffer_size;
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
