// 2024/04/01 10:52:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/LED/driverLED.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


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
        static PinOut pinGREEN(GPIOA, GPIO_PIN_10);
        static PinOut pinBLUE(GPIOA, GPIO_PIN_11);
    }
}


void LED::Driver::Init()
{
    pinRED.Init();
    pinGREEN.Init();
    pinBLUE.Init();

    Off();
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


void LED::Driver::On(Color::E color)
{
    if ((uint8)color > 7)
    {
        return;
    }

    static const uint8 values[8] =
    {
        //    RGB
            0b111,      // White
            0b100,      // Red
            0b010,      // Green
            0b001,      // Blue
            0b110,      // Yellow
            0b011,      // Cyan
            0b101       // Magenta
    };

    pinRED.Set(_GET_BIT(values[color], 2) != 0);
    pinGREEN.Set(_GET_BIT(values[color], 1) != 0);
    pinBLUE.Set(_GET_BIT(values[color], 0) != 0);

    is_fired = true;
}
