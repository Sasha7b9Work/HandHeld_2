// 2024/03/20 08:59:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct ColorLED
{
    enum E
    {
        Red,
        Green,
        Blue,
        Count
    };
};


namespace LED
{
    void Init();

    void Enable(ColorLED::E);

    void Disable();
}
