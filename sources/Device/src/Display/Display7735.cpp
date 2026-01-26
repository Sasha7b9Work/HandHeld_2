// 2024/03/01 22:45:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Modules/ST7735/ST7735.h"
#include "Settings/Settings.h"
#include "Display/Font/Font.h"
#include "Keyboard/Keyboard.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Menu.h"
#include "Utils/StringUtils.h"
#include "Hardware/Power.h"
#include "Utils/Math.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"
#include "Utils/FPS.h"


namespace Display
{
    namespace Buffer
    {
        extern uint8 buffer[SIZE];

        extern int current_part;

        static uint crc[NUMBER_PARTS_HEIGHT] = { 0, 0, 0, 0, 0, 0, 0, 0 };

        static uint CalcualteCRC()
        {
            return Math::CalculateCRC32(buffer, SIZE);
        }
    }

    extern void BeginScene(int num_part);
    void EndScene(int num_parts);
}


void Display::Init()
{
    ST7735::Init();

    Font::SetType(TypeFont::_7);
}


void Display::PrepareToSleep()
{
    ST7735::Disable();

    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        Buffer::crc[i] = 0;                         // ¡ÂÁ ˝ÚÓ„Ó ÌÂ ·Û‰ÂÚ ‚˚ıÓ‰ËÚ¸ ÔÓ ÍÌÓÔÍÂ ËÁ ÒÌ‡
    }
}


void Display::DrawPowerOn()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("¬ Àﬁ◊≈Õ»≈").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::DrawLowVoltage()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("Õ»« Œ≈").WriteInCenter(0, 20, Display::WIDTH, Color::RED);

        Text<>("Õ¿œ–ﬂ∆≈Õ»≈").WriteInCenter(0, 50, Display::WIDTH, Color::RED);

        EndScene(i);
    }
}


void Display::EndScene(int num_parts)
{
    uint crc = Buffer::CalcualteCRC();

    if (crc != Buffer::crc[Buffer::current_part])
    {
        ST7735::Enable();

        Buffer::crc[Buffer::current_part] = crc;

        ST7735::WriteBuffer(HEIGHT / NUMBER_PARTS_HEIGHT * num_parts);
    }
}
