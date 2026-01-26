// 2026/01/26 17:35:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace ST7789
{
    void Init();

    uint TimeEnabled();

    void Disable();

    void Enable();

    // Записывает буфер дисплея в строки, начиная с y
    void WriteBuffer(int y);
}
