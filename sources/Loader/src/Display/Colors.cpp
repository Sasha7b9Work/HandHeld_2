// 2024/03/02 09:35:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"


uint16 Color::colors[16] =
{
    MAKE_COLOR(0x1f, 0x3f, 0x1f),   // WHITE
    MAKE_COLOR(0x1f, 0x00, 0x00),   // RED
    MAKE_COLOR(0x00, 0x3f, 0x00),   // GREEN
    MAKE_COLOR(0x00, 0x00, 0x1f),   // BLUE
    MAKE_COLOR(0x1f, 0x3f, 0x00),   // YELLOW
    MAKE_COLOR(0x00, 0x3f, 0x1f),   // CYAN
    MAKE_COLOR(0x1f, 0x00, 0x1f),   // MAGENTA
    MAKE_COLOR(0x10, 0x20, 0x10),   // GRAY
    MAKE_COLOR(0x00, 0x00, 0x00),   // BLACK
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};


Color Color::WHITE(White);
Color Color::RED(Red);
Color Color::GREEN(Green);
Color Color::BLUE(Blue);
Color Color::YELLOW(Yellow);
Color Color::CYAN(Cyan);
Color Color::MAGENTA(Magenta);
Color Color::GRAY(Gray);
Color Color::BLACK(Black);
Color Color::NUMBER(Count);


Color Color::current = Color::NUMBER;


void Color::SetAsCurrent() const
{
    if (value < Count)
    {
        current = *this;
    }
}


pchar Color::Name(E v)
{
    static const pchar names[Count] =
    {
        "аекши",
        "йпюямши",
        "гекемши",
        "яхмхи",
        "фекрши",
        "цнксани",
        "оспоспмши",
        "яепши",
        "вепмши"
    };

    return names[v];
}


Color::E Color::Contrast(E v)
{
    static const Color::E contrs[Count] =
    {
        Black,
        White,
        Black,
        White,
        Black,
        Black,
        Black,
        Black,
        White
    };

    return contrs[v];
}
