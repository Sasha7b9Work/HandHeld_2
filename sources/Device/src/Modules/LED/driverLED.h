// 2024/04/01 10:51:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/LED/LED.h"


namespace LED
{
    namespace Driver
    {
        void Init();

        void On();

        void On(ColorLED::E);

        void Off();

        // true, если находится в состоянии "On()"
        bool IsFired();
    }
}
