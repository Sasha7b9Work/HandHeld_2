// 2024/03/10 14:08:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"


PinIn pinDOUT(0, 0);
PinOut pinPWR(0, 0);
PinIn pinPWR_CTRL(0, 0);


void PinIn::Init()
{

}


bool PinIn::IsHi() const
{
    return true;
}


void PinOut::Init()
{

}


void PinOut::ToLow() const
{

}


void PinOut::ToHi() const
{

}
