// 2024/03/01 23:01:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Keyboard.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <cstring>


void Keyboard::Init()
{
    btnUp.Init();
    btnDown.Init();
    btnLeft.Init();
    btnRight.Init();
}


bool Keyboard::NeedUpgrade()
{
    return btnLeft.IsLow() || btnRight.IsLow() || btnUp.IsLow();
}


void Keyboard::Update()
{
    if (btnLeft.IsLow())
    {
        int i = 0;
    }

    if (btnRight.IsLow())
    {
        int i = 0;
    }

    if (btnUp.IsLow())
    {
        int i = 0;
    }

    if (btnDown.IsLow())
    {
        int i = 0;
    }
}
