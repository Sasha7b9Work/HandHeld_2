// 2024/04/23 08:38:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display1602.h"
#include "Modules/WH1602B/WH1602B.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Source.h"
#include "Menu/Menu.h"
#include "Hardware/Power.h"
#include "Utils/StringUtils.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include <cstring>


template int Text<64>::Write(int x, int y, const Color &color) const;


namespace Display
{
    namespace CGRAM
    {
        static const int NUM_SYMBOLS = 0x1B;

        struct Symbol
        {
            uint8 rows[7];          // В каждом байте хранятся 5 точек строки символа
        };

        static const Symbol symbols[NUM_SYMBOLS] =
        {
            {
                BINARY_U8(00011111),    // Б - 0x00
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00011111),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00011111),    // Г - 0x01
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00010000)
            },
            {
                BINARY_U8(00001110),    // Д - 0x02
                BINARY_U8(00001010),
                BINARY_U8(00001010),
                BINARY_U8(00001010),
                BINARY_U8(00001010),
                BINARY_U8(00011111),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00010101),    // Ж - 0x03
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00001110),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101)
            },
            {
                BINARY_U8(00001110),    // З - 0x04
                BINARY_U8(00010001),
                BINARY_U8(00000001),
                BINARY_U8(00000110),
                BINARY_U8(00000001),
                BINARY_U8(00010001),
                BINARY_U8(00001110)
            },
            {
                BINARY_U8(00010001),    // И - 0x05
                BINARY_U8(00010001),
                BINARY_U8(00010011),
                BINARY_U8(00010101),
                BINARY_U8(00011001),
                BINARY_U8(00010001),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00010101),    // Й - 0x06
                BINARY_U8(00010001),
                BINARY_U8(00010011),
                BINARY_U8(00010101),
                BINARY_U8(00011001),
                BINARY_U8(00010001),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00000111),    // Л - 0x07
                BINARY_U8(00001001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00011111),    // П - 0x08
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00010001),    // У - 0x09
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00001111),
                BINARY_U8(00000001),
                BINARY_U8(00000001),
                BINARY_U8(00001110)
            },
            {
                BINARY_U8(00001110),    // Ф - 0x0A
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00001110),
                BINARY_U8(00000100),
                BINARY_U8(00000100)
            },
            {
                BINARY_U8(00010001),    // Ц - 0x0B
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111),
                BINARY_U8(00000001)
            },
            {
                BINARY_U8(00010001),    // Ч - 0x0C
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00001111),
                BINARY_U8(00000001),
                BINARY_U8(00000001)
            },
            {
                BINARY_U8(00010101),    // Ш - 0x0D
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00010101),    // Щ - 0x0E
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00011111),
                BINARY_U8(00000001)
            },
            {
                BINARY_U8(00011000),    // Ъ - 0x0F
                BINARY_U8(00001000),
                BINARY_U8(00001000),
                BINARY_U8(00001111),
                BINARY_U8(00001001),
                BINARY_U8(00001001),
                BINARY_U8(00001111)
            },
            {
                BINARY_U8(00010001),    // Ы - 0x10
                BINARY_U8(00010001),
                BINARY_U8(00011101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00011101)
            },
            {
                BINARY_U8(00010000),    // Ь - 0x11
                BINARY_U8(00010000),
                BINARY_U8(00010000),
                BINARY_U8(00011110),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011110)
            },
            {
                BINARY_U8(00011110),    // Э - 0x12
                BINARY_U8(00000001),
                BINARY_U8(00000001),
                BINARY_U8(00001111),
                BINARY_U8(00000001),
                BINARY_U8(00000001),
                BINARY_U8(00011110)
            },
            {
                BINARY_U8(00010111),    // Ю - 0x13
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00011101),
                BINARY_U8(00010101),
                BINARY_U8(00010101),
                BINARY_U8(00010111)
            },
            {
                BINARY_U8(00001111),    // Я - 0x14
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00001111),
                BINARY_U8(00000101),
                BINARY_U8(00001001),
                BINARY_U8(00010001)
            },
            {
                BINARY_U8(00001110),    // Пустой аккумулятор - 0x15
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00001110),    // 1/5 линий - 0x16
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00001110),    // 2/5 линий - 0x17
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00001110),    // 3/5 линий - 0x18
                BINARY_U8(00010001),
                BINARY_U8(00010001),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00001110),    // 4/5 линий - 0x19
                BINARY_U8(00010001),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111)
            },
            {
                BINARY_U8(00001110),    // 5/5 линий - 0x1A
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111),
                BINARY_U8(00011111)
            },

        };

        struct SlotCGRAM
        {
            uint8 code;     // Код символа от 9 до 30 - столько символов нет в стандартном знакогенераторе
            uint  time;     // Время загузки символа. Нужно, чтобы определить символ, который загружался раньше всех,
                            // чтобы на его место записать другой, более нужный
        };

        static SlotCGRAM slots[8] =
        {
            { 255, 0 },
            { 255, 0 },
            { 255, 0 },
            { 255, 0 },
            { 255, 0 },
            { 255, 0 },
            { 255, 0 },
            { 255, 0 }
        };

        static void Clear()
        {
            for (int i = 0; i < 8; i++)
            {
                slots[i].code = 255;
            }
        }

        // Возвращает true, если символ с кодом code загружен в CGRAM.
        static bool SymbolLoaded(uint8 code, int *slot)
        {
            for (int i = 0; i < 8; i++)
            {
                if (slots[i].code == code)
                {
                    *slot = i;

                    return true;
                }
            }

            return false;
        }

        // Загрузить символ в память дисплея в слот, который использовался раньше всех
        static int LoadSymbol(uint8 code)
        {
            int slot = 0;                  // Место размешение символа в CGRAM - от 0 до 7

            uint time = 0xFFFFFFFF;

            for (int i = 0; i < 8; i++)
            {
                if (slots[i].time < time)
                {
                    slot = i;
                    time = slots[i].time;
                }
            }

            slots[slot] = { code, TIME_MS };

            WH1602B::LoadSymbolToCGRAM(slot, symbols[code].rows);

            return slot;
        }
    };

    // Основной буфер. Сюда пишем
    static char buffer[2][16];

    // Буфер предыдущего экрана. Если 
    static char prev_buffer[2][16] =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    static void BeginScene();
    static void EndScene();
    // Конвертировать выводимые символы в символы, приходные для загрузки в дисплй
    static void Convert();
    // Загрузить в дисплей символы, которых нет в стандартном знакогенераторе и заменить на них
    // нужные символы в буфере
    static void LoadSymbolsToCGRAM();
    static void WriteSymbol(int x, int y, char);
}


void Display::DrawSymbol(int x, int y, uint8 symbol)
{
    WriteSymbol(x, y, (char)symbol);
}


void Display::Init()
{
    WH1602B::Enable();
}


void Display::Update()
{
    BeginScene();

    if (PCF8563::IsAlarmed())
    {
        Text<>("БУДИЛЬНИК").WriteInCenter(0, 0, Display::WIDTH, Color(Color::Contrast(gset.alarm.color)));
    }
    else if (Source::GetCountReceived())
    {
        pchar name = Source::Name(Source::Current());

        int num_words = SU::NumWordsInString(name);

        if (num_words == 1)
        {
            Text<>(name).WriteInCenter(0, 0, Display::WIDTH);
        }
        else if (num_words == 2)
        {
            char buff[32];

            Text<>(SU::GetWordFromString(name, 1, buff)).WriteInCenter(0, 0, Display::WIDTH);

            Text<>(SU::GetWordFromString(name, 2, buff)).WriteInCenter(0, 1, Display::WIDTH);
        }
    }
    else
    {
        if (Menu::IsShown())
        {
            Menu::Draw();
        }
        else
        {
            PCF8563::GetDateTime().DrawTime(0, 0);

            PCF8563::GetDateTime().DrawDate(0, 0);

            Power::Draw();
        }
    }

    EndScene();
}


void Display::DrawPowerOn()
{
    BeginScene();

    Text<>("ВКЛЮЧЕНИЕ").WriteInCenter(0, 0, Display::WIDTH);

    EndScene();
}


void Display::DrawPowerOff()
{
    BeginScene();

    Text<>("ВЫКЛЮЧЕНИЕ").WriteInCenter(0, 0, Display::WIDTH);

    EndScene();
}


void Display::DrawLowVoltage()
{
    BeginScene();

    Text<>("НИЗКОЕ").WriteInCenter(0, 0, Display::WIDTH);
    Text<>("НАПРЯЖЕНИЕ").WriteInCenter(0, 1, Display::WIDTH);

    EndScene();
}


uint Display::TimeEnabled()
{
    return WH1602B::TimeEnabled();
}


void Display::BeginScene()
{
    std::memset(buffer, ' ', 32);
}


void Display::EndScene()
{
    WH1602B::Enable();

    Convert();

    LoadSymbolsToCGRAM();

    if (std::memcmp(buffer, prev_buffer, 2 * 16) == 0)
    {
        return;
    }

    std::memcpy(prev_buffer, buffer, 2 * 16);

    WH1602B::WriteFull((uint8 *)buffer);
}


void Display::PrepareToSleep()
{
    std::memset(prev_buffer, 0, 16 * 2);

    CGRAM::Clear();

    WH1602B::Disable();
}


void Display::Convert()
{
    //  Б - 0x00
    //  Г - 0x01
    //  Д - 0x02
    //  Ж - 0x03
    //  З - 0x04
    //  И - 0x05
    //  Й - 0x06
    //  Л - 0x07
    //  П - 0x08
    //  У - 0x09
    //  Ф - 0x0A
    //  Ц - 0x0B
    //  Ч - 0x0C
    //  Ш - 0x0D
    //  Щ - 0x0E
    //  Ъ - 0x0F
    //  Ы - 0x10
    //  Ь - 0x11
    //  Э - 0x12
    //  Ю - 0x13
    //  Я - 0x14

    static const pchar symbs =
        "++++++++++++++++"
        "++++++++++++++++"
        "++++++++++++++++"
        "++++++++++++++++"
        "A\x00""B\x01\x02""E\x03\x04\x05\x06K\x07MHO\x08"           // 0xA0
        "PCT\x09\x0AX\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14"      // 0xD0
        "++++++++++++++++"                                          // 0xE0
        "\x15\x16\x17\x18\x19\x1A+++++++++++";                      // 0xF0     // Состояния аккумулятора - от пустого к полному

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            uint8 symbol = (uint8)buffer[i][j];

            if (symbol > 127)
            {
                buffer[i][j] = symbs[symbol - 128];
            }
        }
    }

    // На выходе сиволы кириллицы будут замены на символы латинского алфавита (если есть),
    // либо на коды символов, загружаемых в дисплей
}


void Display::LoadSymbolsToCGRAM()
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            uint8 symbol = (uint8)buffer[i][j];

            if (symbol <= CGRAM::NUM_SYMBOLS)
            {
                int slot = 0;

                if (!CGRAM::SymbolLoaded(symbol, &slot))
                {
                    slot = CGRAM::LoadSymbol(symbol);
                }

                buffer[i][j] = (char)slot;
            }
        }
    }
}


void RTCDateTime::DrawTime(int, int, const Color &) const
{
    Text<>("%02d:%02d", Hour, Minute).WriteInCenter(0, 0, Display::WIDTH);
}


void RTCDateTime::DrawDate(int, int, const Color &) const
{
    Text<>("%02d/%02d/%02d", Day, Month, Year).WriteInCenter(0, 1, Display::WIDTH);
}


template<int capacity>
int Text<capacity>::Write(int x, int y, const Color &) const
{
    if (x == -1)
    {
        WriteInCenter(0, y, 0);
    }
    else
    {
        const char *pointer = &text[0];

        while (*pointer)
        {
            Display::WriteSymbol(x++, y, *pointer);
            pointer++;
        }
    }

    return 0;
}


void Display::WriteSymbol(int x, int y, char symbol)
{
    if (y == 0 || y == 1)
    {
        if (x >= 0 && x <= 15)
        {
            buffer[y][x] = symbol;
        }
    }
}
