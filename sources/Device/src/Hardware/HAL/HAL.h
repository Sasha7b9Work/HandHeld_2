// 2024/03/01 22:24:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SPI_PAN3060 SPI1


struct Settings;


struct ModeClock
{
    enum E
    {
        Sleep,      // Спящий режим
        Low,        // Основная частота 3MHz
        Hi,         // Частота 52МГц для отрисовки экрана
        Count
    };

    static void Set(E);

    static void LeaveSleepMode();

    static bool IsSleep()     { return current == Sleep; }
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


// Для часов
namespace HAL_I2C
{
    void Init();

    bool Write(uint8 command, uint8 *data, int size);

    bool Read(uint8 reg, uint8 *buf, uint16 len);
}


namespace HAL_ROM
{
    static const uint ADDRESS_BEGIN = 0x8000000;

#ifdef MODEL7735
    static const uint SIZE_PAGE = 1024;
    static const uint NUM_PAGES = 64;
    static const int PAGE_FOR_JOURNAL = 63;
    static const int PAGE_FOR_SETTINGS = 62;
#endif

#ifdef MODEL7789
    static const uint SIZE_PAGE = 2048;
    static const uint NUM_PAGES = 128;
    static const int PAGE_FOR_JOURNAL = 127;
    static const int PAGE_FOR_SETTINGS = 126;

#endif

    // Стиреть страницу от 0 до (NUM_PAGES - 1)
    void ErasePage(int);

    uint AddressPage(int);

    // address должен быть кратен 4
    void WriteBuffer(uint address, const void *buffer, int size);

    // address должен быть кратен 4
    void ReadBuffer(uint address, void *buffer, int size);

    bool LoadSettings(Settings *);

    void SaveSettings(Settings *);
}
