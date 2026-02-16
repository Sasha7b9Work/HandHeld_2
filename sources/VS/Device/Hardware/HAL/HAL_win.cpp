// 2024/03/02 13:19:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


ModeClock::E ModeClock::current = ModeClock::Low;


void ModeClock::Set(E)
{

}


void ModeClock::LeaveDeepSleep()
{

}


void HAL::Init()
{
    HAL_ROM::Init();
}

