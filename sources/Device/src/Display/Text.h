// 2024/05/13 11:05:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Display/Font/Font.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>



struct Char
{
    Char(char s) : symbol((uint8)s) { }
    int Write(int x, int y, const Color &color = Color::NUMBER) const;
private:
    uint8 symbol;
};


template<int capacity = 64>
struct Text
{
    Text(pchar format, ...)
    {
        std::va_list args;
        va_start(args, format);
        std::vsprintf(text, format, args);
        va_end(args);
    }
    int Write(int x, int y) const;
    int Write(int x, int y, const Color &) const;
    void WriteInCenter(int x, int y, int width, const Color &color)
    {
        color.SetAsCurrent();
        WriteInCenter(x, y, width);
    }
    void WriteInCenter(int x, int y, int width) const
    {
        int length = GetLength();

        if (length < width)
        {
            x += width / 2 - length / 2;
        }

        Write(x, y);
    }
    int GetLength() const
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
    char *c_str()
    {
        return &text[0];
    }
private:
    char text[capacity];
};
