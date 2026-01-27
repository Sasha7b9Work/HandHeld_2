// 2026/01/26 16:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "system.h"


void HAL_ROM::ErasePage(int num_page)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_page_erase(AddressPage(num_page));

    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    fmc_lock();
}


uint HAL_ROM::AddressPage(int num_page)
{
    return ADDRESS_BEGIN + (uint)num_page * SIZE_PAGE;
}


void HAL_ROM::WriteBuffer(uint address, const void *buffer, int size)
{
    fmc_unlock();

    const uint *data = (const uint *)buffer;

    for (uint i = 0; i < (uint)size; i += 4)
    {
        fmc_word_program(address, *data);

        address += 4;

        data++;

        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }

    fmc_lock();
}


void HAL_ROM::ReadBuffer(uint address, void *buffer, int size)
{
    uint *data = (uint *)buffer;

    for (uint i = 0; i < (uint)size; i += 4)
    {
        *data = *((uint *)address);

        data++;

        address += 4;
    }
}


bool HAL_ROM::LoadSettings(Settings *set)
{
    ReadBuffer(AddressPage(PAGE_FOR_SETTINGS), set, sizeof(*set));

    return set->crc32 == set->CalculateCRC32();
}


void HAL_ROM::SaveSettings(Settings *set)
{
    Settings control_set;

    do
    {
        set->crc32 = set->CalculateCRC32();

        set->empty = 0;

        ErasePage(PAGE_FOR_SETTINGS);

        WriteBuffer(AddressPage(PAGE_FOR_SETTINGS), set, sizeof(*set));

    } while (!LoadSettings(&control_set));
}
