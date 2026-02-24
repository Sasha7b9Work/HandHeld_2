// 2024/04/13 20:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SU
{
    int NumWordsInString(pchar);

    // Нумерация начинается с 1
    char *GetWordFromString(pchar, int, char out[32]);

    namespace Y
    {
        static int Center()
        {
#ifdef MODEL7735
            return 30;
#else
            return 70;
#endif
        }
    }
}
