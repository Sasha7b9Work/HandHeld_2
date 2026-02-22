// 2026/02/22 17:18:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Primitives.h"
#include "Display/Display.h"
#include "Display/Text.h"
#include "Modules/PCF8563/PCF8563.h"


template int Text<64>::Write(int x, int y, const Color &) const;
template int Text<64>::Write(int x, int y) const;
template void Text<64>::WriteInCenter(int x, int y, int width, const Color &) const;
template void Text<64>::WriteInCenter(int x, int y, int width) const;
template int Text<64>::GetLength() const;

namespace Display
{
    namespace Buffer
    {
        extern int current_part;
        extern uint8 buffer[SIZE];
    }
}


void Pixel::Set(int x, int y) const
{
    if (x < 0)
    {
        return;
    }

    if (x >= Display::WIDTH)
    {
        return;
    }

    y -= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT * Display::Buffer::current_part;

    if (y < 0)
    {
        return;
    }

    if (y >= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT)
    {
        return;
    }

    Display::Buffer::buffer[y * Display::WIDTH + x] = (uint8)Color::current.value;
}


void Pixel::Set(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    Set(x, y);
}


void HLine::Draw(int x, int y) const
{
    if (x >= Display::WIDTH)
    {
        return;
    }

    y -= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT * Display::Buffer::current_part;

    if (y < 0)
    {
        return;
    }

    if (y >= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT)
    {
        return;
    }

    uint8 *pixel = Display::Buffer::buffer + y * Display::WIDTH + x;

    for (int i = 0; i < width; i++)
    {
        *pixel++ = (uint8)Color::current.value;

        x++;

        if (x >= Display::WIDTH)
        {
            break;
        }
    }
}


void VLine::Draw(int x, int y) const
{
    for (int i = 0; i < height; i++)
    {
        Pixel().Set(x, y++);
    }
}


void Rect::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    HLine(width).Draw(x, y);
    HLine(width).Draw(x, y + height - 1);
    VLine(height).Draw(x, y);
    VLine(height).Draw(x + width - 1, y);
}


void Rect::Fill(int x0, int y0) const
{
    for (int y = y0; y < y0 + height; y++)
    {
        HLine(width).Draw(x0, y);
    }
}


void Rect::Fill(int x0, int y0, const Color &color) const
{
    color.SetAsCurrent();

    Fill(x0, y0);
}


void RTCDateTime::DrawTime(int x, int y, const Color &color) const
{
    Text<>("%02d:%02d", Hour, Minute).Write(x, y, color);
}


void RTCDateTime::DrawDate(int x, int y, const Color &color) const
{
    Text<>("%02d/%02d/%02d", Day, Month, Year).Write(x, y, color);
}

template<int capacity>
int Text<capacity>::Write(int x, int y) const
{
    pchar pointer = text;

    while (*pointer)
    {
        x = Char(*pointer++).Write(x, y);
        x += Font::GetSize(); //-V1026
    }

    return x;
}


template<int capacity>
int Text<capacity>::Write(int x, int y, const Color &color) const
{
    color.SetAsCurrent();
    return Write(x, y);
}

template<int capacity>
void Text<capacity>::WriteInCenter(int x, int y, int width, const Color &color) const
{
    color.SetAsCurrent();
    WriteInCenter(x, y, width);
}

template<int capacity>
void Text<capacity>::WriteInCenter(int x, int y, int width) const
{
    int length = GetLength();

    if (length < width)
    {
        x += width / 2 - length / 2;
    }

    Write(x, y);
}

template<int capacity>
int Text<capacity>::GetLength() const
{
    int result = 0;

    pchar pointer = text;

    while (*pointer)
    {
        uint8 symbol = (uint8)*pointer++;
        result += Font::GetWidth(symbol) * Font::GetSize(); //-V1026
        result += Font::GetSize(); //-V1026
    }

    return result;
}
