// 2024/05/15 19:51:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/LED/driverLED.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>


/*
    RED   - PA9
    GREEN - PA10
    BLUE  - PA11

*/

namespace LED
{
    namespace Driver
    {
        static bool is_fired = false;

        static PinOut pinRED(GPIOA, GPIO_PIN_9);
#ifdef BOARD_NEW
        static PinOut pinGREEN(GPIOA, GPIO_PIN_10);
        static PinOut pinBLUE(GPIOA, GPIO_PIN_11);
#else
        static PinOut pinGREEN(GPIOA, GPIO_PIN_11);
        static PinOut pinBLUE(GPIOA, GPIO_PIN_10);
#endif
    }
}


void LED::Driver::Init()
{
    pinRED.Init();
    pinGREEN.Init();
    pinBLUE.Init();

    Off();
}


void LED::Driver::On(ColorLED::E color)
{
    Off();

    if (color == ColorLED::Red)
    {
        pinRED.ToHi();
    }
    else if (color == ColorLED::Green)
    {
        pinGREEN.ToHi();
    }
    else if (color == ColorLED::Blue)
    {
        pinBLUE.ToHi();
    }

    is_fired = true;
}


void LED::Driver::Off()
{
    is_fired = false;

    pinRED.ToLow();
    pinGREEN.ToLow();
    pinBLUE.ToLow();
}


bool LED::Driver::IsFired()
{
    return is_fired;
}
