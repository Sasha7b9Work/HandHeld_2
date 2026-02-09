// 2024/03/03 17:06:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "system.h"


namespace Beeper
{
    namespace Driver
    { 
        // PB0 - Alternate TIMER2_CH2 AF_1

        static const uint PORT = GPIOB;
        static const uint PIN = GPIO_PIN_0;
        static const uint TIMER = TIMER2;
#ifdef MODEL7735
        static const uint TIMER_CHAN = TIMER_CH_2;
#endif
#ifdef MODEL7789
        static const uint TIMER_CHAN = TIMER_CH_3;
#endif

        void Init();

        // first == true - первый запуск, false - запуск следующей ноты
        void StartFrequency(float frequency, uint8 vol, bool first);

        void Stop();
    }
}


void Beeper::Driver::Init()
{
#ifdef MODEL7735
    gpio_mode_set(PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN);
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);
    gpio_af_set(PORT, GPIO_AF_1, PIN);
#endif

#ifdef MODEL7789
    gpio_init(PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, PIN);
#endif

    timer_deinit(TIMER);

    timer_parameter_struct timer_initpara =
    {
        // TIMER2CLK = SystemCoreClock / 18 = 4MHz, the PWM frequency is 16000 Hz
#ifdef MODEL7735
        71,
#endif
#ifdef MODEL7789
        119,
#endif
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        0,
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
//    timer_channel_output_pulse_value_config(TIMER, TIMER_CHAN, 12);
    timer_channel_output_mode_config(TIMER, TIMER_CHAN, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER, TIMER_CHAN, TIMER_OC_SHADOW_DISABLE);

    //    timer_primary_output_config(TIMER, ENABLE);

        /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER);
    //    timer_interrupt_enable(TIMER14, TIMER_INT_CH1);
    //    timer_enable(TIMER14);
}


void Beeper::Driver::StartFrequency(float frequency, uint8 vol, bool first)
{
    if (first)
    {
#ifdef MODEL7735
        gpio_mode_set(PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, PIN);
        gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);
        gpio_af_set(PORT, GPIO_AF_1, PIN);
#endif

#ifdef MODEL7789
        gpio_init(PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, PIN);
#endif
    }

    timer_disable(TIMER);

    /*
    *   1000 - 1000 √ц
    *   2000 -  500 √ц
    *   4000 -  250 √ц
    *
    *   period = 1000 / X√ц * 1000
    */

    uint period = (uint)((1000.0f / (frequency + 0.5f) * 1000.0f) + 0.5f);

    TIMER_CAR(TIMER) = period;

    if (vol == 2)
    {
#ifdef MODEL7735
        TIMER_CH2CV(TIMER) = period * 50 / 100;
#endif
#ifdef MODEL7789
        TIMER_CH3CV(TIMER) = period * 50 / 100;
#endif
    }
    else if (vol == 1)
    {
#ifdef MODEL7735
        TIMER_CH2CV(TIMER) = 32;
#endif
#ifdef MODEL7789
        TIMER_CH3CV(TIMER) = 32;
#endif
    }
    else if (vol == 0)
    {
#ifdef MODEL7735
        TIMER_CH2CV(TIMER) = 4;
#endif
#ifdef MODEL7789
        TIMER_CH3CV(TIMER) = 4;
#endif
    }

    if (first)
    {
#ifdef MODEL7735
        TIMER_DMAINTEN(TIMER) |= (uint32_t)TIMER_INT_CH2;
#endif
#ifdef MODEL7789
        TIMER_DMAINTEN(TIMER) |= (uint32_t)TIMER_INT_CH3;
#endif
    }

    timer_enable(TIMER);
}


void Beeper::Driver::Stop()
{
#ifdef MODEL7735
    timer_interrupt_disable(TIMER, TIMER_INT_CH2);
#endif
#ifdef MODEL7789
    timer_interrupt_disable(TIMER, TIMER_INT_CH3);
#endif
    timer_disable(TIMER);

#ifdef MODEL7735
    gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);
    gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN);
#endif

#ifdef MODEL7789
    gpio_init(PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PIN);
#endif

    gpio_bit_reset(PORT, PIN);                  // ѕереводим в ноль, чтобы не палить динамик
}
