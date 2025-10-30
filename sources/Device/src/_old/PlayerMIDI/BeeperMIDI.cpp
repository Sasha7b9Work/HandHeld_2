// 2024/03/03 17:06:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "PlayerMIDI/BeeperMIDI.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "PlayerMIDI/PlayerMIDI.h"
#include <gd32e23x.h>


namespace BeeperMIDI
{
    // PA3 - Alternate TIMER14_CH1 AF0
}


void BeeperMIDI::Init()
{
    rcu_periph_clock_enable(RCU_TIMER14);
    nvic_irq_enable(TIMER14_IRQn, 0);

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_3);

    timer_deinit(TIMER14);

    timer_parameter_struct timer_initpara =
    {
        // TIMER14CLK = SystemCoreClock / 36 = 1MHz, the PWM frequency is 4000 Hz
        17,
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        249,
        0
    };

    timer_init(TIMER14, &timer_initpara);

    timer_oc_parameter_struct timer_ocinitpara =
    {
        TIMER_CCX_ENABLE,
        TIMER_CCXN_DISABLE,
        TIMER_OC_POLARITY_HIGH,
        TIMER_OCN_POLARITY_HIGH,
        TIMER_OC_IDLE_STATE_LOW,
        TIMER_OCN_IDLE_STATE_LOW
    };
    timer_channel_output_config(TIMER14, TIMER_CH_1, &timer_ocinitpara);

    /* CH1 configuration in PWM mode0, duty cycle 50% */
    timer_channel_output_pulse_value_config(TIMER14, TIMER_CH_1, 125);
    timer_channel_output_mode_config(TIMER14, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER14, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER14, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER14);
    timer_interrupt_enable(TIMER14, TIMER_INT_CH1);
    timer_enable(TIMER14);
}


void BeeperMIDI::CallbackOnOutputSample(uint8 sample)
{
    TIMER_CH1CV(TIMER14) = (uint32_t)sample;
}


void BeeperMIDI::CallbackOnStartMelody()
{

}


void BeeperMIDI::CallbackOnStopMelody()
{
    PlayerMIDI::Play(TypeMelody::_1);
}
