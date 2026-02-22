// 2024/03/01 22:45:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/Font/Font.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Display
{
#ifdef MODEL7735
    static const int WIDTH = 160;
    static const int HEIGHT = 80;
#endif

#ifdef MODEL7789
    static const int WIDTH = 320;
    static const int HEIGHT = 240;
#endif

    static const int NUMBER_PARTS_HEIGHT = 8;          // На столько частей разбит дислей. Столько раз будем отрисовывать изображение,
                                                // чтобы получить полную картинку

    void Init();

    void Update();

    void Refresh();

    void PrepareToSleep();

    void DrawPowerOff();

    void DrawPowerOn();

    void DrawLowVoltage();

    uint TimeEnabled();

    bool IsOldType();

    namespace Buffer
    {
        static const uint SIZE = WIDTH * HEIGHT / NUMBER_PARTS_HEIGHT;

        // Возвращает указатель на строку
        uint8 *GetLine(int y);
    }
}
