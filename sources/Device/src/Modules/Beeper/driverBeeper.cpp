// 2024/03/03 17:06:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Beeper/driverBeeper.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include <gd32e23x.h>


namespace Beeper
{
    namespace Driver
    {
        // PB0 - Alternate TIMER2_CH2 AF_1

        static const uint PORT = GPIOB;
        static const uint PIN = GPIO_PIN_0;
        static const uint TIMER = TIMER2;
        static const uint TIMER_CHAN = TIMER_CH_2;
    }
}


void Beeper::Driver::Init()
{
    gpio_mode_set(PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN);
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);
    gpio_af_set(PORT, GPIO_AF_1, PIN);

    timer_deinit(TIMER);

    timer_parameter_struct timer_initpara =
    {
        // TIMER2CLK = SystemCoreClock / 18 = 4MHz, the PWM frequency is 16000 Hz
        8,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        249,
        0
    };

    timer_init(TIMER, &timer_initpara);

    timer_oc_parameter_struct timer_ocinitpara =
    {
        TIMER_CCX_ENABLE,
        TIMER_CCXN_DISABLE,
        TIMER_OC_POLARITY_HIGH,
        TIMER_OCN_POLARITY_HIGH,
        TIMER_OC_IDLE_STATE_LOW,
        TIMER_OCN_IDLE_STATE_LOW
    };
    timer_channel_output_config(TIMER, TIMER_CHAN, &timer_ocinitpara);

    /* CH1 configuration in PWM mode0, duty cycle 50% */
    timer_channel_output_pulse_value_config(TIMER, TIMER_CHAN, 12);
    timer_channel_output_mode_config(TIMER, TIMER_CHAN, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER, TIMER_CHAN, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER);
//    timer_interrupt_enable(TIMER14, TIMER_INT_CH1);
//    timer_enable(TIMER14);
}


void Beeper::Driver::StartFrequency(float frequency, uint8 vol)
{
    gpio_mode_set(PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN);
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);
    gpio_af_set(PORT, GPIO_AF_1, PIN);

    uint period = 250;

    if (vol == 1)
    {
        period = 150;
    }
    else if (vol == 0)
    {
        period = 125;
    }

    uint16 prescaler = (uint16)(SystemCoreClock / period / (uint)(frequency + 0.5f));

    TIMER_PSC(TIMER) = prescaler;
    TIMER_CAR(TIMER) = period/4;

    TIMER_DMAINTEN(TIMER) |= (uint32_t)TIMER_INT_CH2;

    TIMER_CTL0(TIMER) |= (uint32_t)TIMER_CTL0_CEN;
}


void Beeper::Driver::Stop()
{
    timer_interrupt_disable(TIMER, TIMER_INT_CH2);
    timer_disable(TIMER);

    gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);

    gpio_bit_reset(PORT, PIN);                  // Переводим в ноль, чтобы не палить динамик
}


void Beeper::Driver::CallbackOnOutputSample(uint8 sample)
{
    TIMER_CH1CV(TIMER) = (uint32_t)sample;
}
