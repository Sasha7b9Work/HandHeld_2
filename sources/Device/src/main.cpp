// 2024/03/01 22:30:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by// 
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Display/Display7735.h"
#include "Hardware/Timer.h"
#include <cstdlib>


// Ammyy      95 243 113     32766



void Update();

static void Update2();

static void UpdateLED();


int main()
{
    HAL::Init();

    Display::Init();

    //    Power::Init();

    Timer::Init();

//    ModeClock::Set(ModeClock::Hi);

    while (true)
    {
        Update2();
    }
}


void Update2()
{
    UpdateLED();

    Display::UpdateDebug();
}


void UpdateLED()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 1000)
    {
        meter.Reset();

        static int color = 0;

        color++;

        color &= 0x07;
    }
}
