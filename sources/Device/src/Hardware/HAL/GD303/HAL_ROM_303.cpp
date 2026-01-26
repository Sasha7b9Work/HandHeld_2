// 2026/01/26 16:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


void HAL_ROM::SaveSettings(Settings *)
{

}


bool HAL_ROM::LoadSettings(Settings *)
{
    return false;
}


void HAL_ROM::WriteBuffer(uint /*address*/, const void * /*buffer*/, int /*size*/)
{

}


uint HAL_ROM::AddressPage(int)
{
    return 0;
}


void HAL_ROM::ErasePage(int)
{

}


void HAL_ROM::ReadBuffer(uint /*address*/, void * /*buffer*/, int /*size*/)
{

}
