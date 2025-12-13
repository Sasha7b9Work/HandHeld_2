// 2025/12/13 22:15:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Utils/String.h"


/*
    В этот файл вынесены некоторые функции из PAN3060.cpp, чтобы иметь возможность их отлаживать
*/


namespace PAN3060
{
    extern bool in_process_upgrade;
}


void PAN3060::Firmware::CheckForComplete()
{
    if (!IsFilled())
    {
        return;
    }

    uint crc_real = SU::CalculateCRC32((const void *)BEGIN_FIRMWARE, NUM_PAGES * 1024);

    if (crc_real == crc)
    {
        in_process_upgrade = false;
    }
    else
    {
        Clear();
        page.Clear();
    }
}

