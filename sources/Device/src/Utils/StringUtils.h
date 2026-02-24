// 2024/04/13 20:33:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SU
{
    int NumWordsInString(pchar);

    // Нумерация начинается с 1
    char *GetWordFromString(pchar, int, char out[32]);

    namespace Y
    {
        // Одно слово рисуем по центру
        static int Center()
        {
#ifdef MODEL7735
            return 30;
#else
            return 70;
#endif
        }

        // Две строки на экране. Координата верхней
        static int Up()
        {
#ifdef MODEL7735
            return 15;
#else
            return 30;
#endif
        }

        // Две строки на экране. Координата нижней
        static int Down()
        {
#ifdef MODEL7735
            return 45;
#else
            return 120;
#endif
        }

#ifdef MODEL7789
        namespace Str
        {
            static int Up()
            {
                return 15;
            }

            static int Center()
            {
                return 85;
            }

            static int Down()
            {
                return 160;
            }
        }
#endif
    }
}
