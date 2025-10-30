// 2023/04/17 14:03:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Font/Font.h"
#include "Display/Font/font5.inc"
#include "Display/Font/font8.inc"
#include "Display/Text.h"
#include "Display/Font/font10_7.inc"
#include "Display/Display7735.h"

//static const DFont *fonts[DTypeFont::Count] = { &font5, &font8 };



static const DFont *dfont = &font8;

static FontDef font_10x7 = { 7, 10, Font10x7 };

namespace Font
{
    TypeFont::E type = TypeFont::_7;

    static int size = 1;
}


void Font::SetSize(int _size)
{
    size = _size;
}


int Font::GetSize()
{
    return size;
}


void Font::SetType(TypeFont::E _type)
{
    type = _type;

    if (_type == TypeFont::_5)
    {
        dfont = &font5;
    }
    else if (_type == TypeFont::_7)
    {
        dfont = &font8;
    }
}


int Font::GetBit(uint8 symbol, int row, int bit)
{
    return dfont->symbols[symbol].bytes[row] & (1 << (7 - bit));
}

int Font::GetHeight()
{
    switch (type)
    {
    case TypeFont::_5:      return 5;
    case TypeFont::_7:      return 7;
    case TypeFont::_10:     return 10;
    case TypeFont::Count:
        break;
    }

    return 10;
}


int Font::GetWidth(uint8 symbol)
{
    return dfont->symbols[symbol].width;
}


#ifdef TYPE_1602
int Char::Write(int x, int, const Color & /* = Color::NUMBER */) const
{
    return x;
}
#else
int Char::Write(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    if (Font::type == TypeFont::_5 || Font::type == TypeFont::_7)
    {
        int height = Font::GetHeight();
        int width = Font::GetWidth(symbol);

        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                if (Font::GetBit(symbol, row, col))
                {
                    Rect(Font::size, Font::size).Fill(x + col * Font::size, y + row * Font::size);
                }
            }
        }

        return x + width * Font::size;
    }
    else if (Font::type == TypeFont::_10)
    {
        if (symbol < 32 || symbol > 126)
        {
            return 0;
        }

        for (int i = 0; i < font_10x7.height; i++)
        {
            int b = font_10x7.data[(symbol - 32) * font_10x7.height + i];

            for (int j = 0; j < font_10x7.width; j++)
            {
                if ((b << j) & 0x8000)
                {
                    Rect(Font::size, Font::size).Fill(x + j * Font::size, y + i * Font::size);
                }
            }
        }

        return x + font_10x7.width * Font::size;
    }

    return x;
}
#endif
