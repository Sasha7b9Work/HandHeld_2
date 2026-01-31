// 2024/03/02 09:35:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Colors.h"
#include "Display/Display.h"


uint16 Color::colors[16] =
{
    0x0000,   // WHITE
    0x0000,   // RED
    0x0000,   // GREEN
    0x0000,   // BLUE
    0x0000,   // YELLOW
    0x0000,   // CYAN
    0x0000,   // MAGENTA
    0x0000,   // GRAY
    0x0000,   // BLACK
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


void Color::Init()
{
    colors[0] = Make(0x1f, 0x3f, 0x1f);   // WHITE
    colors[1] = Make(0x1f, 0x00, 0x00);   // RED
    colors[2] = Make(0x00, 0x3f, 0x00);   // GREEN
    colors[3] = Make(0x00, 0x00, 0x1f);   // BLUE
    colors[4] = Make(0x1f, 0x3f, 0x00);   // YELLOW
    colors[5] = Make(0x00, 0x3f, 0x1f);   // CYAN
    colors[6] = Make(0x1f, 0x00, 0x1f);   // MAGENTA
    colors[7] = Make(0x10, 0x20, 0x10);   // GRAY
    colors[8] = Make(0x00, 0x00, 0x00);   // BLACK
}


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


uint16 Color::Make(uint8 r, uint8 g, uint8 b)
{
    if (Display::IsOldType())
    {
        return (uint16)((b)+((g) << 5) + ((r) << 11));
    }
    else
    {
        return (uint16)((0x1f - (b)) + ((0x3f - (g)) << 5) + ((0x1f - (r)) << 11));
    }
}
