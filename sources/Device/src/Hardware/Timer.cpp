// 2024/03/02 09:30:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/systick.h"
#include "system.h"


uint timer_counter = 0;


void Timer::Init()
{
#ifdef MODEL7735
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

#endif

#ifdef MODEL7789

    #pragma message("Function Timer::Init not defined")

#endif
}


void Timer::Delay(uint timeMS)
{
    delay_1ms(timeMS);
}


void TimeMeterMS::Reset()
{
    in_pause = false;

    time_reset = timer_counter;
}


uint TimeMeterMS::ElapsedTime() const
{
    if (in_pause)
    {
        uint time_reset_temp = time_reset;
        time_reset_temp += (timer_counter - time_begin_pause);

        return timer_counter - time_reset_temp;
    }

    return timer_counter - time_reset;
}


void TimeMeterMS::Pause()
{
    in_pause = true;

    time_begin_pause = timer_counter;
}


void TimeMeterMS::Resume()
{
    in_pause = false;

    time_reset += (timer_counter - time_begin_pause);
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
