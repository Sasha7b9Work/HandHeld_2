// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct DSymbol
{
    uchar width;
    uchar bytes[8];
};

struct DTypeFont
{
    enum E
    {
        _5,
        _8,
        Count,
        None
    };
};

struct DFont
{
    int _height;
    DSymbol symbols[256];

public:
    static int  GetLengthText(pchar text);

    // Устанавливает текущий шрифт. Ранее установленный можно восстановить функцией Pop()
    static void Set(const DTypeFont::E typeFont);

    static DTypeFont::E Current();

    // Восстанавливает шрифт, бывший текущим перед последним вызовом SetCurrent()
    static void Pop();

    static bool IsBig();

    static uint8 GetWidth(uint8 symbol);

    static uint8 GetWidth(char symbol);

    static uint8 GetHeight();

    static bool RowNotEmpty(uint8 symbol, int row);

    static bool BitIsExist(uint8 symbol, int row, int bit);

    static void SetMinWidth(uint8 width);

    // Устанавливает количество пустых пикселей между символами.
    static void SetSpacing(int spacing = 1);

    // Возвращает количество пустых пикселей между символами
    static int GetSpacing();
};

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


struct FontDef
{
    const uint8 width;      // Font width in pixels
    uint8 height;           // Font height in pixels
    const uint16 *data;     // Pointer to data font data array
};


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

