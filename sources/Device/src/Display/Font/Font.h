// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Font/FontBig.h"
#include "Display/Font/FontMid.h"


struct TypeFont
{
    enum E
    {
#ifndef MODEL7789
        _5,
        _7,
#endif
#ifdef MODEL7789
        GOSTAU16BOLD,
        GOSTB28B,
#endif
        _Count
    };
};


namespace Font
{
    void SetType(TypeFont::E);
    void SetMainType();
    void SetSmallType();

    void SetSize(int);

    int GetSize();

    int GetBit(uint8 symbol, int row, int col);

    int GetHeight();

    int GetWidth(uint8 symbol);

    extern TypeFont::E type;

#ifdef MODEL7789
    bool RowNotEmpty(uint8 symbol, int row);
    bool BitIsExist(uint8 symbol, int row, int bit);
#endif
}

