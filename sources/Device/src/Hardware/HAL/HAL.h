// 2024/03/01 22:24:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SPI_PAN3060 SPI1


struct ModeClock
{
    enum E
    {
        DeepSleep,  // Спящий режим
        Low,        // Основная частота 3MHz
        Hi,         // Частота 52МГц для отрисовки экрана
        Count
    };

    static void _Set(E);

    static void LeaveDeepSleep();

    static bool IsDeepSleep() { return current == DeepSleep; }
    static bool IsLow()       { return current == Low; }
    static bool IsHi()        { return current == Hi; }

private:

    static E current;
};


namespace HAL
{
    void Init();

    void DeInit();
}


// Для дисплея
namespace HAL_I2C
{
    void Init();

    bool Write(uint8 command, uint8 *data, int size);

    bool Read(uint8 reg, uint8 *buf, uint16 len);
}
