// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Font/FontBig.h"
#include "Display/Font/FontMid.h"


struct TypeFont
{
    enum E
    {
#ifdef MODEL7735
        _5,
        _7,
#else
        Small,
        Main,
#endif
        Count
    };
};


namespace Font
{
    void SetMainType();
    void SetSmallType();

    TypeFont::E CurrentType();

    // Сохраняем установленный тип, чтобы восстановить его позднее
    void StoreType();
    // Восстанавливаем ранее сохранённый тип
    void RestoreType();

    // Размер одного пикселя. Т.е. если 2, то размер символа будет в два раза больше оригинального
    void SetSize(int);
    int GetSize();

    int GetBit(uint8 symbol, int row, int col);

    int GetHeight();

    int GetWidth(uint8 symbol);

    // Расстояние между символами в пикселях
    int LetterSpacing();

#ifdef MODEL7789
    bool RowNotEmpty(uint8 symbol, int row);
    bool BitIsExist(uint8 symbol, int row, int bit);
#endif
}

