// 2024/03/20 08:59:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Source.h"


struct ColorLED
{
    enum E
    {
        Red,
        Green,
        Blue,
        Count
    };
};


namespace LED
{
    void Init();

    void Update();

    void Enable();

    void Enable(ColorLED::E);

    void Disable();

    ColorLED::E CurrentColor();

    // true, если в данный момент черзе светодиод идёт ток (повышенное потребление)
    bool IsFired();
}
