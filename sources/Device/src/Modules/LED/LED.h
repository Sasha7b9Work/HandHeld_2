// 2024/03/20 08:59:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Source.h"


namespace LED
{
    void Init();

    void Update();

    void Enable();

    void Disable();

    // true, если в данный момент черзе светодиод идёт ток (повышенное потребление)
    bool IsFired();
}
