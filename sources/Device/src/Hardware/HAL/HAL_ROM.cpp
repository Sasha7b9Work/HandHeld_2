// 2024/03/20 20:10:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32e23x.h>


namespace HAL_ROM
{
    static void Unlock()
    {
        if ((RESET != (FMC_CTL & FMC_CTL_LK)))
        {
            /* write the FMC key */
            FMC_KEY = UNLOCK_KEY0;
            FMC_KEY = UNLOCK_KEY1;
        }
    }


    static fmc_state_enum StateGet()
    {
        fmc_state_enum fmc_state = FMC_READY;

        if ((uint)0x00U != (FMC_STAT & FMC_STAT_BUSY)) {
            fmc_state = FMC_BUSY;
        }
        else {
            if ((uint)0x00U != (FMC_STAT & FMC_STAT_WPERR)) {
                fmc_state = FMC_WPERR;
            }
            else {
                if ((uint)0x00U != (FMC_STAT & FMC_STAT_PGERR)) {
                    fmc_state = FMC_PGERR;
                }
            }
        }
        /* return the FMC state */
        return fmc_state;
    }


    static fmc_state_enum ReadyWait(uint timeout)
    {
        fmc_state_enum fmc_state = FMC_BUSY;

        /* wait for FMC ready */
        do {
            /* get FMC state */
            fmc_state = StateGet();
            timeout--;
        } while ((FMC_BUSY == fmc_state) && (0U != timeout));

        if (FMC_BUSY == fmc_state) {
            fmc_state = FMC_TOERR;
        }
        /* return the FMC state */
        return fmc_state;
    }

    static fmc_state_enum WordProgram(uint address, uint data)
    {
        fmc_state_enum fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

        if (FMC_READY == fmc_state) {
            /* set the PG bit to start program */
            FMC_CTL |= FMC_CTL_PG;

            REG32(address) = data;
            /* wait for the FMC ready */
            fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

            /* reset the PG bit */
            FMC_CTL &= ~FMC_CTL_PG;
        }

        /* return the FMC state */
        return fmc_state;
    }
}


void HAL_ROM::ErasePage(int num_page)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    fmc_page_erase(AddressPage(num_page));

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    fmc_lock();
}


uint HAL_ROM::AddressPage(int num_page)
{
    return ADDRESS_BEGIN + (uint)num_page * SIZE_PAGE;
}


void HAL_ROM::WriteBuffer(uint address, const void *buffer, int size)
{
    Unlock();

    const uint *data = (const uint *)buffer;

    for (uint i = 0; i < (uint)size; i += 4)
    {
        WordProgram(address, *data);

        address += 4;

        data++;

        FMC_STAT = FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR;
    }

    FMC_CTL |= FMC_CTL_LK;
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
