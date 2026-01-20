// 2026/01/20 13:52:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/GlobalFunctions.h"


bool GF::MemCmp(const void *_a0, const void *_a1, int size)
{
    const uint8 *a0 = (const uint8 *)_a0;
    const uint8 *a1 = (const uint8 *)_a1;

    for (int i = 0; i < size; i++)
    {
        if (*a0++ != *a1++)
        {
            return false;
        }
    }

    return true;
}
