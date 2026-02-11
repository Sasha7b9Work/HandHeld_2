// 2026/02/10 22:47:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/MonochromeBitmap.h"
#include "Display/Display.h"


void MonochromeBitmap::Draw(int x, int y, bool inverse)
{
    if (inverse)
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                if (GetPoint(i, j))
                {
                    Pixel().Set(x + i, y + j);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
                if (!GetPoint(i, j))
                {
                    Pixel().Set(x + i, y + j);
                }
            }
        }
    }
}
