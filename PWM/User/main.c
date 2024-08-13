#include "debug.h"
#include "string.h"
#include "I2C_LCD.h"


#define ADC_cnt_max                 1023
#define ADC_REF                     3300
#define voltage_divider_ratio         10

#define Target_Output_Voltage       3400
#define PWM_Duty_Max                 500


u8 adc_EOC_complete = RESET;
u8 adc_JEOC_complete = RESET;


void GPIO_init(void);
void ADC_init(void);
void TIM1_PWM_Out_init(u16 period, u16 prescalar, u16 ccp_value);
void ADC1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


void ADC1_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
    {
        adc_EOC_complete = SET;
        GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }

    if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET)
    {
        adc_JEOC_complete = SET;
        GPIO_SetBits(GPIOD, GPIO_Pin_4);
        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
    }
}


void main(void)
{
    char buffer[10];

    u8 i = 0;
    u8 j = 0;
    u16 in = 0;
    u16 out = 0;
    u16 in_V = 0;
    u16 out_V = 0;
    u16 in_buffer[4];
    u16 out_buffer[4];
    s16 pwm_duty = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_init();
    ADC_init();
    TIM1_PWM_Out_init(5000, 1, 0);
    LCD_init();
    LCD_clear_home();
    LCD_putstr(0, 0, "VIN (mV):");
    LCD_putstr(0, 1, "PWM Duty:");

    while(1)
    {
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        Delay_Ms(40);

        if(adc_EOC_complete)
        {
            in_buffer[i] = ADC_GetConversionValue(ADC1);
            adc_EOC_complete = RESET;
        }

        ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);

        if(adc_JEOC_complete)
        {
            out_buffer[i] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
            i++;
            adc_JEOC_complete = RESET;
        }

        if(i >= 4)
        {
            for(j = 0; j < 4; j++)
            {
                in += in_buffer[j];
                out += out_buffer[j];
            }

            in >>= 2;
            out >>= 2;

            memset(in_buffer, 0x0000, 4);
            memset(out_buffer, 0x0000, 4);

            in_V = ((in * ADC_REF * voltage_divider_ratio) / ADC_cnt_max);
            out_V = ((out * ADC_REF * voltage_divider_ratio) / ADC_cnt_max);


            if(out_V < Target_Output_Voltage)
            {
                pwm_duty++;
            }

            if(pwm_duty >= PWM_Duty_Max)
            {
                pwm_duty = PWM_Duty_Max;
            }

            if(out_V >= Target_Output_Voltage)
            {
                pwm_duty--;
            }

            if(pwm_duty <= 0)
            {
                pwm_duty = 0;
            }

            TIM_SetCompare1(TIM1, pwm_duty);

            in = 0;
            out = 0;

            i = 0;
        }

        sprintf(buffer, "%4d ", in_V);
        LCD_putstr(11, 0, buffer);

        sprintf(buffer, "%4d ", pwm_duty);
        LCD_putstr(11, 1, buffer);

        Delay_Ms(10);
    };
}


void GPIO_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void ADC_init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

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

    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_241Cycles);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_241Cycles);

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

}


void TIM1_PWM_Out_init(u16 period, u16 prescalar, u16 ccp_value)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1), ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );

    TIM_TimeBaseInitStructure.TIM_Period = period;
    TIM_TimeBaseInitStructure.TIM_Prescaler = (prescalar - 1);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp_value;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure );

    TIM_CtrlPWMOutputs(TIM1, ENABLE );
    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_ARRPreloadConfig( TIM1, ENABLE );
    TIM_Cmd( TIM1, ENABLE );
}
