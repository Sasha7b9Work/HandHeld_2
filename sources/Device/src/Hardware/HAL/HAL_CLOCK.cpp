// 2026/04/02 20:15:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Keyboard/Keyboard.h"
#include "Settings/Source.h"
#include "system.h"


ModeClock::E ModeClock::current = ModeClock::Low;


namespace HAL_CLOCK
{
    bool in_sleep_mode = false;
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
