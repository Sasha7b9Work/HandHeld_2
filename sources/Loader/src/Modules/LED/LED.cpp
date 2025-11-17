// 2024/03/20 08:59:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Modules/LED/LED.h"
#include "Modules/LED/driverLED.h"


namespace LED
{
    static bool is_enabled = false;

    static uint time_enable = 0;
}


void LED::Init()
{
    Driver::Init();

    Disable();
}


void LED::Enable(ColorLED::E color)
{
    if (!is_enabled)
    {
        is_enabled = true;

        time_enable = TIME_MS;

        Driver::On(color);
    }
}


void LED::Disable()
{
    if (is_enabled)
    {
        is_enabled = false;

        Driver::Off();
    }
}
