// 2025/11/22 12:20:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "upgrader/Timer.h"


uint32_t counter_ms = 0;


static uint32_t time_reset[32];


uint32_t Timer_CurrentTime()
{
    return counter_ms;
}


void Timer_Reset(int num_timer)
{
    time_reset[num_timer] = TIME_MS;
}


uint32_t Timer_ElapsedMS(int num_timer)
{
    return TIME_MS - time_reset[num_timer];
}
