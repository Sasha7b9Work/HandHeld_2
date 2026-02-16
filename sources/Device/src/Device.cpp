// 2026/02/16 09:34:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/ST7735_89/ST7735_89.h"
#include "Settings/Settings.h"
#include "Hardware/Power.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Keyboard/Keyboard.h"
#include "Menu/Menu.h"
#include "Modules/LED/LED.h"
#include "Hardware/Vibrato.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/Beeper/Beeper.h"
#include "Storage/Storage.h"
#include "Modules/PMS150G/PMS150G.h"


namespace Device
{
    static void SaveSettings();
}


void Device::Init()
{
    HAL::Init();

    ST7735_89::BeforeInit();

    gset.Load();

    Power::Init();

    Display::Init();

    PCF8563::Init();

    Timer::Init();

    Keyboard::Init();

    Menu::Init();

    LED::Init();

    Vibrato::Init();

    PAN3060::InitFull();

    Beeper::Init();

    Storage::Init();

    PMS150G::Init();

    LED::ShortWhiteBlink();
}


void Device::Update()
{
    if (Keyboard::ToMoreTime() &&
        Source::GetCountReceived() == 0 &&
        !PCF8563::IsAlarmed())
    {
#ifndef MODEL7789
        ModeClock::Set(ModeClock::Sleep);

        ModeClock::LeaveSleepMode();

        Power::MeasVoltage();
#endif
    }

    ModeClock::Set(ModeClock::Hi);

    PCF8563::Update();

    Menu::Update();

    PAN3060::Update();

    if (Source::GetCountReceived() || !Keyboard::ToMoreTime())
    {
        Display::Update();
    }

    LED::Update();

    Vibrato::Update();

    Beeper::Update();

    Source::Update();

    SaveSettings();

    Power::Update();
}


void Device::SaveSettings()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 5000)
    {
        meter.Reset();

        gset.Save();
    }
}
