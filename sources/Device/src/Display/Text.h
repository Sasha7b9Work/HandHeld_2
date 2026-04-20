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
    int Write(int x, int y) const;
    int Write(int x, int y, const Color &) const;
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

    int _Write(int x, int y, const Color & = Color::NUMBER) const;
    void WriteInCenter(int x, int y, int width, const Color & = Color::NUMBER) const;
    int GetLength() const;
    char *c_str()
    {
        return &text[0];
    }
    const char *c_str() const
    {
        return &text[0];
    }
private:
    char text[capacity];
};
