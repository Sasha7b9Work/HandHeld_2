// 2024/04/01 10:51:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace LED
{
    namespace Driver
    {
        void Init();

        void On();

        void Off();

        // true, если находится в состоянии "On()"
        bool IsFired();
    }
}
