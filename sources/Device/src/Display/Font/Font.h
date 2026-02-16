// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Font/FontBig.h"
#include "Display/Font/FontMid.h"


struct TypeFont
{
    enum E
    {
        _5,
        _7,
#ifdef MODEL7789
        GOSTAU16BOLD,
        GOSTB28B,
#endif
        Count
    };
};


#ifdef MODEL7735
    #define S_4_5   4
    #define S_5_6   5
#endif

#ifdef MODEL7789
    #define S_4_5   4
    #define S_5_6   5
#endif


namespace Font
{
    void SetType(TypeFont::E);

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

