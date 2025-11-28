// 2024/03/20 08:59:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ColorLED
{
    enum E
    {
        Black,
        Red,
        Green,
        _3,
        Blue,
        _5,
        _6,
        White,
        Count
    };
};


namespace LED
{
    void Init();

    void Enable(ColorLED::E);

    void Disable();
}
