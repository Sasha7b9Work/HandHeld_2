// 2024/03/01 22:47:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace ST7735
{
    void Init();

    void Fill(uint16);

    // Записывает буфер дисплея в строки, начиная с y
    void WriteBuffer(int y);

    void Enable();

    void Disable();

    bool IsEnabled();

    uint TimeEnabled();
}
