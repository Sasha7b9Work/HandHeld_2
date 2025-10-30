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
};



#define MAKE_COLOR(r, g, b)  (uint16)((b) + ((g) << 5) + ((r) << 11))

#define RED_FROM_COLOR(color)   (((color) >> 11) & 0x1f)
#define GREEN_FROM_COLOR(color) (((color) >> 5) & 0x3f)
#define BLUE_FROM_COLOR(color)  ((color) & 0x1f)
