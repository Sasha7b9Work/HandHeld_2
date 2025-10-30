// 2024/03/02 09:30:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/systick.h"
#include <gd32e23x.h>


uint timer_counter = 0;


void Timer::Init()
{
    rcu_periph_clock_enable(RCU_TIMER13);
    timer_deinit(TIMER13);                          // Будем использовать для подсчёта микросекунд

    timer_parameter_struct timer_initpara =
    {
        (uint16)((SystemCoreClock / 1000000) - 1),
        TIMER_COUNTER_EDGE,
        TIMER_COUNTER_UP,
        TIMER_CKDIV_DIV1,
        uint(-1) - 1,
        0
    };

    timer_init(TIMER13, &timer_initpara);
}


void Timer::Delay(uint timeMS)
{
    delay_1ms(timeMS);
}


void TimeMeterMS::Reset()
{
    time_reset = timer_counter;
}


uint TimeMeterMS::ElapsedTime() const
{
    return timer_counter - time_reset;
}


uint TimerUS::ElaplsedTime()
{
    return TIMER_CNT(TIMER13);
}


void TimerUS::Reset()
{
    TIMER_CNT(TIMER13) = 0;
    TIMER_CTL0(TIMER13) |= (uint32_t)TIMER_CTL0_CEN;
}
