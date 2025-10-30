// 2024/03/01 22:45:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/Font/Font.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Display
{
    static const int WIDTH = 160;
    static const int HEIGHT = 80;

    static const int NUMBER_PARTS_HEIGHT = 8;          // Ќа столько частей разбит дислей. —только раз будем отрисовывать изображение,
                                                // чтобы получить полную картинку

    void Init();

    void Update();

    void Update2();

    void PrepareToSleep();

    void DrawPowerOff();

    void DrawPowerOn();

    void DrawLowVoltage();

    uint TimeEnabled();

    namespace Buffer
    {
        static const uint SIZE = WIDTH * HEIGHT / NUMBER_PARTS_HEIGHT;

        // ¬озвращает указатель на строку
        uint8 *GetLine(int y);
    }
}


struct Pixel
{
    void Set(int x, int y, const Color &color = Color::NUMBER) const;
};


struct HLine
{
    HLine(int w) : width(w) { }
    void Draw(int x, int y, const Color &color = Color::NUMBER) const;
private:
    const int width;
};


struct VLine
{
    VLine(int h) : height(h) { }
    void Draw(int x, int y, const Color &color = Color::NUMBER) const;
private:
    const int height;
};


struct Rect
{
    Rect(int w, int h) : width(w), height(h) { }
    void Draw(int x, int y, const Color &color = Color::NUMBER) const;
    void Fill(int x, int y, const Color &color = Color::NUMBER) const;
private:
    const int width;
    const int height;
};
