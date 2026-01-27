// 2026/01/26 17:35:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/DriverST/DriverST.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/DriverST/driver_st7789.h"
#include "Hardware/Timer.h"
#include "Display/Display.h"


extern PinOut pinDC_RS;
extern PinOut pinRES;
extern PinOut pinBKG;
extern PinOut pinON;


namespace DriverST
{
    static bool is_enabled = false;

    static uint time_enable = 0;
}


void DriverST::Init()
{
    ST7789_Init();
}


uint DriverST::TimeEnabled()
{
    return TIME_MS - time_enable;
}


void DriverST::Disable()
{
    if (!is_enabled)
    {
        return;
    }

    pinBKG.ToLow();

    ST7789_WriteCommand(0x10);  // Sleep In

    is_enabled = false;
}


void DriverST::Enable()
{
    if (is_enabled)
    {
        return;
    }

    time_enable = TIME_MS;

    ST7789_WriteCommand(0x11);  // Sleep Out

    pinBKG.ToHi();

    is_enabled = true;
}


void DriverST::WriteBuffer(int y0)
{
    ST7789_WriteBuffer(0, (int16)y0, Display::WIDTH - 1, (int16)(y0 + Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT - 1), Display::Buffer::GetLine(y0));
}
