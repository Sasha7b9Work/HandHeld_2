// 2026/02/22 17:17:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"


struct Pixel
{
    void Set(int x, int y) const;
    void Set(int x, int y, const Color &) const;
};


struct HLine
{
    HLine(int w) : width(w)
    {
    }
    void Draw(int x, int y) const;
private:
    const int width;
};


struct VLine
{
    VLine(int h) : height(h)
    {
    }
    void Draw(int x, int y) const;
private:
    const int height;
};


struct Rect
{
    Rect(int w, int h) : width(w), height(h)
    {
    }
    void Draw(int x, int y) const;
    void Draw(int x, int y, const Color &) const;
    void Fill(int x, int y) const;
    void Fill(int x, int y, const Color &) const;
private:
    const int width;
    const int height;
};

