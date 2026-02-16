// 2024/03/23 11:36:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace HAL_ROM
{
    static uint8 memory[NUM_PAGES * SIZE_PAGE];
}


void HAL_ROM::Init()
{
    std::memset(memory, 0xFF, NUM_PAGES * HAL_ROM::SIZE_PAGE);
}


void HAL_ROM::ErasePage(int number)
{
    for (uint address = number * HAL_ROM::SIZE_PAGE; address < (number + 1) * HAL_ROM::SIZE_PAGE; address++)
    {
        memory[address] = 0xFF;
    }
}


void HAL_ROM::WriteBuffer(uint address, const void *buffer, int size)
{
    const uint8 *data = (const uint8 *)buffer;

    for (uint i = 0; i < (uint)size; i++)
    {
        memory[address + size] = *data++;
    }
}


uint HAL_ROM::AddressPage(int number)
{
    return number * SIZE_PAGE;
}


void HAL_ROM::ReadBuffer(uint address, void *buffer, int size)
{
    uint8 *data = (uint8 *)buffer;

    for (uint i = 0; i < (uint)size; i++)
    {
        *data++ = memory[address + size];
    }
}
