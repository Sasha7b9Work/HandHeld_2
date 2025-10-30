// 2024/04/29 21:47:46 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace WH1602B
{
    void Enable();

    void Disable();

    void Clear();

    // Записать все символы дисплея
    void WriteFull(uint8[32]);

    // Записать в CGARAM символ с кодом code
    void LoadSymbolToCGRAM(int slot, const uint8 rows[7]);

    uint TimeEnabled();
}

