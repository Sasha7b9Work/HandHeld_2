// 2024/03/20 08:59:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include "Modules/LED/LED.h"
#include "Modules/LED/driverLED.h"


namespace LED
{
    static bool is_enabled = false;

    static uint time_enable = 0;

    static ColorLED::E current_color = ColorLED::Red;
}


void LED::Init()
{
    Driver::Init();

    Disable();
}


void LED::Update()
{
    if (!is_enabled)
    {
        return;
    }

    Source::E source = PCF8563::IsAlarmed() ? Source::Test : Source::Current();

    if (SourceScript::GetForLED(source, TIME_MS - time_enable))
    {
        Driver::On();
    }
    else
    {
        Driver::Off();
    }
}


void LED::Enable()
{
    if (!is_enabled)
    {
        is_enabled = true;

        time_enable = TIME_MS;
    }
}


void LED::Enable(ColorLED::E color)
{
    current_color = color;

    is_enabled = true;

    Driver::On(color);
}


ColorLED::E LED::CurrentColor()
{
    return current_color;
}


void LED::Disable()
{
    if (is_enabled)
    {
        is_enabled = false;

        Driver::Off();
    }
}


bool LED::IsFired()
{
    return Driver::IsFired();
}
