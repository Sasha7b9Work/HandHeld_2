// 2024/04/13 20:34:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/StringUtils.h"
#include <cstring>


int SU::NumWordsInString(pchar string)
{
    int result = 0;

    char buffer[128];

    std::strcpy(buffer, string);

    char *pointer = strtok(buffer, " ");

    while (pointer != nullptr)
    {
        result++;
        pointer = strtok(nullptr, " ");
    }

    return result;
}


char *SU::GetWordFromString(pchar string, int num, char out[32])
{
    if (NumWordsInString(string) < num)
    {
        out[0] = '\0';
    }
    else
    {
        char buffer[128];
        std::strcpy(buffer, string);

        char *pointer = strtok(buffer, " ");

        num--;

        while (pointer != nullptr)
        {
            if (num == 0)
            {
                std::strcpy(out, pointer);

                return out;
            }

            pointer = strtok(nullptr, " ");
            num--;
        }
    }

    return out;
}
