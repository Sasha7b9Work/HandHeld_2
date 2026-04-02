// 2026/04/02 20:15:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Keyboard/Keyboard.h"
#include "Settings/Source.h"
#include "system.h"


ModeClock::E ModeClock::current = ModeClock::Low;


namespace HAL_CLOCK
{
    extern void SetSleepMode();
    extern void SetLow();
    extern void SetHi();

    static bool in_sleep_mode = false;
}


void ModeClock::Set(E v)
{
    if (v == ModeClock::Sleep)
    {
        if (!ModeClock::IsSleep())
        {
            current = ModeClock::Sleep;

            HAL_CLOCK::in_sleep_mode = true;

            HAL_CLOCK::SetSleepMode();
        }
    }
    else if (v == ModeClock::Low)
    {
        if (!ModeClock::IsLow())
        {
            current = ModeClock::Low;

            HAL_CLOCK::SetLow();
        }
    }
    else if (v == ModeClock::Hi)
    {
        if (!ModeClock::IsHi())
        {
            current = ModeClock::Hi;

            HAL_CLOCK::SetHi();
        }
    }
}


void ModeClock::LeaveSleepMode()
{
    if (HAL_CLOCK::in_sleep_mode)
    {
        HAL_CLOCK::in_sleep_mode = false;

        if (Keyboard::ActionExist())                    // Если проснулись от кнопки
        {
            ModeClock::Set(Source::ExistReceived() ? ModeClock::Hi : ModeClock::Low);
        }
//        else                                            // А здесь проснулись от приёмника
//        {
//            PAN3060::InitOn90ms();
//        }
    }
}
