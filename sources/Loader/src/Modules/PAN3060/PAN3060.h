// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace PAN3060
{
    void Init();

    void FuncDraw();

    void CallbackOnIRQ();

    // Идёт процесс обновления
    bool InProcessUpgrade();

    // Описывает одну страницу
    struct Page
    {
        struct Chain
        {
            static const uint SIZE_CHAIN = 128;
            uint8 buffer[SIZE_CHAIN];
        };

        static const int CHAINS_IN_PAGE = 8;

        Chain chains[CHAINS_IN_PAGE];

        bool received[CHAINS_IN_PAGE];

        int number = -1;

        void Clear();

        // true, если страница полностью заполнена
        bool IsFilled() const;

        // Записывает page[1024] в EEPROM
        void WritePageEEPROM() const;
    };

    // Эта структура описывает все данные прошивки
    struct Firmware
    {
        static const uint NUM_PAGES = 54;
        static const uint BEGIN_FIRMWARE = 0x8002000;

        bool pages[NUM_PAGES];      // true означает, что страница принята и сохранена в EEPROM
        uint crc = 0;

        void CheckForComplete();

        bool IsFilled() const;

        void Clear();

        int FilledPages() const
        {
            int result = 0;

            for (uint i = 0; i < NUM_PAGES; i++)
            {
                if (pages[i])
                {
                    result++;
                }
            }

            return result;
        }
    };

    // Принимаемая прошивка
    extern Firmware firmware;

    // Принимаемая страница
    extern Page page;
}
