// 2022/03/30 08:25:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


namespace SU
{
    uint CalculateCRC32(const void *buffer, int size);

    char *IntToASCII(int, char[32]);
}
