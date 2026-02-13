// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct TypeFont
{
    enum E
    {
        _5,
        _7,
        _10,
        Count
    };
};


#ifdef MODEL7735
    #define S_2_3   2
    #define S_4_5   4
    #define S_5_6   5
#endif

#ifdef MODEL7789
    #define S_2_3   2
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
}

