// 2024/05/15 19:51:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/LED/driverLED.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Settings/Settings.h"
#include <gd32e23x.h>


/*
    RED   - PB7
    GREEN - PB8
    BLUE  - PB9

*/

namespace LED
{
    namespace Driver
    {
        static bool is_fired = false;

        static PinOut pinRED(GPIOB, GPIO_PIN_7);
        static PinOut pinGREEN(GPIOB, GPIO_PIN_8);
        static PinOut pinBLUE(GPIOB, GPIO_PIN_9);

        static bool enabled[7][3] =
        {
            { true,  true,  true  },    // white
            { true,  false, false },    // red
            { false, true,  false },    // green
            { false, false, true  },    // blue
            { true,  true,  false },    // yellow
            { false, true,  true  },    // cyan
            { true,  false, true  }     // magenta
        };

        static const PinOut *pins[3] = { &pinRED, &pinGREEN, &pinBLUE };
    }
}


void LED::Driver::Init()
{
    pinRED.Init();
    pinGREEN.Init();
    pinBLUE.Init();

    Off();
}


void LED::Driver::On()
{
    if (Source::GetCountReceived())
    {
        is_fired = true;

        Color::E color = gset.sources[Source::Current()].color;

        if (color < 7)
        {
            for (int i = 0; i < 3; i++)
            {
                pins[i]->Set(!enabled[color][i]);
            }
        }
    }
}


void LED::Driver::Off()
{
    is_fired = false;

    pinRED.ToHi();
    pinGREEN.ToHi();
    pinBLUE.ToHi();
}


bool LED::Driver::IsFired()
{
    return is_fired;
}
