// 2024/03/20 09:21:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "system.h"


namespace HAL_ADC
{
    static float ConversionRawToVoltageADC(uint);

    static float ConversionRawToVoltageBattery(uint);

    // Эта функция не вынесена в общий доступ потому, что её нельзя вызывать когда попало - надо вызывать только когда потребление не
    // сильно высоко, чтобы ёмкость замерить более-менее адекватно
    float GetVoltage();
}


void HAL_ADC::Init()
{
#ifdef MODEL7735
    rcu_periph_clock_enable(RCU_ADC);

    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_PULLDOWN, GPIO_PIN_2);

    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);

    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);

    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);

    adc_regular_channel_config(0U, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    adc_resolution_config(ADC_RESOLUTION_12B);

    adc_enable();

    Timer::Delay(1U);

    adc_calibration_enable();

    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
#endif

#ifdef MODEL7789
    rcu_periph_clock_enable(RCU_ADC0);

    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_2);

    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);

    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);

    adc_regular_channel_config(ADC0, 0U, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

    adc_enable(ADC0);

    Timer::Delay(1U);

    adc_calibration_enable(ADC0);

    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

#endif
}


float HAL_ADC::GetVoltage()
{
    static float voltage = 5.0f;

#ifdef MODEL7735
    adc_flag_clear(ADC_FLAG_EOC);

    while (SET != adc_flag_get(ADC_FLAG_EOC))
    {
    }

    voltage = ConversionRawToVoltageBattery(ADC_RDATA);
#endif

#ifdef MODEL7789
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

    while (SET != adc_flag_get(ADC0, ADC_FLAG_EOC))
    {
    }

    adc_flag_clear(ADC0, ADC_FLAG_EOC);

    voltage = ConversionRawToVoltageBattery(ADC_RDATA(ADC0));
#endif

    return voltage;
}


float HAL_ADC::ConversionRawToVoltageADC(uint raw)
{
    static const float MAX_VOLTAGE = 3.3f;

    return MAX_VOLTAGE / (1 << 12) * (float)raw;
}


float HAL_ADC::ConversionRawToVoltageBattery(uint raw)
{
    float voltageADC = ConversionRawToVoltageADC(raw);

    return voltageADC * 2.0f;
}
