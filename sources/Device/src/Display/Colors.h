// 2024/03/02 09:35:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Color
{
    enum E
    {
        White,      // R + G + B
        Red,        // R
        Green,      // G
        Blue,       // B
        Yellow,     // R + G
        Cyan,       // G + B    Голубой
        Magenta,    // R + B    Пурпурный
        Gray,
        Black,
        Count = 16
    };

    Color(E v) : value(v) { }

    E value;

    static Color WHITE;
    static Color RED;
    static Color GREEN;
    static Color BLUE;
    static Color YELLOW;
    static Color CYAN;
    static Color MAGENTA;
    static Color GRAY;
    static Color BLACK;

    static Color NUMBER;

    static uint16 colors[Count];

    void SetAsCurrent() const;

    static pchar Name(E);

    // Возвращает контрастный цвет
    static Color::E Contrast(E);

    static Color current;

    static uint16 Make(uint8 r, uint8 g, uint8 b);

    static void Init();

    static uint8 GetRed(uint color)
    {
        return  (((color) >> 11) & 0x1f);
    }

    static uint8 GetGreen(uint color)
    {
        return (((color) >> 5) & 0x3f);
    }

    static uint8 GetBlue(uint color)
    {
        return ((color) & 0x1f);
    }
};
