// 2024/05/13 11:05:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Font/Font.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>



struct Char
{
    Char(char s) : symbol((uint8)s) { }
    int Write(int x, int y) const;
private:
    uint8 symbol;
};


template<int capacity = 64>
struct Text
{
    Text(pchar _text)
    {
        std::strcpy(text, _text);
    }
    int Write(int x, int y) const;
    char *c_str()
    {
        return &text[0];
    }
private:
    char text[capacity];
};
