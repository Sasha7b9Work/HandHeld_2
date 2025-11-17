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


bool Keyboard::NeedUpdate()
{
    return btnLeft.IsLow() || btnRight.IsLow() || btnUp.IsLow();
}
