// 2025/11/28 16:34:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#ifdef GD32E230
	#include <gd32e23x.h>
#endif
#ifdef GD32F303
	#include <gd32f30x.h>
#endif


namespace HAL_ROM
{
    static void WriteUInt(uint address, uint value);

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

        if ((uint)0x00U != (FMC_STAT & FMC_STAT_BUSY))
        {
            fmc_state = FMC_BUSY;
        }
        else
        {
            if ((uint)0x00U != (FMC_STAT & FMC_STAT_WPERR))
            {
                fmc_state = FMC_WPERR;
            }
            else
            {
                if ((uint)0x00U != (FMC_STAT & FMC_STAT_PGERR))
                {
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
        do
        {
            /* get FMC state */
            fmc_state = StateGet();
            timeout--;
        } while ((FMC_BUSY == fmc_state) && (0U != timeout));

        if (FMC_BUSY == fmc_state)
        {
            fmc_state = FMC_TOERR;
        }
        /* return the FMC state */
        return fmc_state;
    }

    static fmc_state_enum PageErase(uint page_address)
    {
        fmc_state_enum fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

        if (FMC_READY == fmc_state)
        {
            /* start page erase */
            FMC_CTL |= FMC_CTL_PER;
            FMC_ADDR = page_address;
            FMC_CTL |= FMC_CTL_START;

            /* wait for the FMC ready */
            fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

            /* reset the PER bit */
            FMC_CTL &= ~FMC_CTL_PER;
        }

        /* return the FMC state  */
        return fmc_state;
    }
}


void HAL_ROM::WriteUInt(uint address, uint value)
{
    fmc_state_enum fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

    if (FMC_READY == fmc_state)
    {
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;

        REG32(address) = value;
        /* wait for the FMC ready */
        fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }
}


void HAL_ROM::ErasePage(uint address_page)
{
    Unlock();

    FMC_STAT = FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR;

    PageErase(address_page);

    FMC_STAT = FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR;

    FMC_CTL |= FMC_CTL_LK;
}


void HAL_ROM::WritePage(uint address, const uint8 bytes[1024])
{
    Unlock();

    uint *data = (uint *)bytes;

    for (int i = 0; i < 1024; i += 4)
    {
//        WriteUInt(address, *data);
    fmc_state_enum fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

    if (FMC_READY == fmc_state)
    {
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;

        REG32(address) = *data;
        /* wait for the FMC ready */
        fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }
		
        address += 4;

        data++;

        FMC_STAT = FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR;
    }

    FMC_CTL |= FMC_CTL_LK;
}
