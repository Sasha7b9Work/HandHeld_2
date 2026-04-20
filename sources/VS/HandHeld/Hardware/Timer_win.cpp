// 2024/03/02 13:21:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include <time.h>


void TimeMeterMS::Reset()
{
    time_reset = Timer::CurrentTime();
}


uint TimeMeterMS::ElapsedTime() const
{
    return Timer::CurrentTime() - time_reset;
}


uint Timer::CurrentTime()
{
    uint result = (uint)((float)clock() / ((float)CLOCKS_PER_SEC / 1000.0f));

    return result;
}


void Timer::Delay(uint timeMS)
{
    uint end_time = CurrentTime() + timeMS;

    while (end_time < CurrentTime())
    {
    }
}
