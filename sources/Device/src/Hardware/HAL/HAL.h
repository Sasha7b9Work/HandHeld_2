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

    static void Set(E);

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


// Уровень батареи
namespace HAL_ADC
{
    void Init();

    float GetVoltage(bool force);
}


// Для дисплея
namespace HAL_I2C
{
    void Init();

    bool Write(uint8 command, uint8 *data, int size);

    bool Read(uint8 reg, uint8 *buf, uint16 len);
}


namespace HAL_ROM
{
    static const uint ADDRESS_BEGIN = 0x8000000;
    static const uint SIZE_PAGE = 1024;
    static const uint NUM_PAGES = 64;

    static const int PAGE_FOR_JOURNAL = 63;

    void Init();

    // Стиреть страницу от 0 до 63
    void ErasePage(int);

    uint AddressPage(int);

    // address должен быть кратен 4
    void WriteBuffer(uint address, const void *buffer, int size);

    // address должен быть кратен 4
    void ReadBuffer(uint address, void *buffer, int size);
}
