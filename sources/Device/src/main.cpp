// 2024/03/01 22:30:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by// 
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Hardware/Timer.h"
#include "Keyboard/Keyboard.h"
#include "Menu/Menu.h"
#include "Modules/LED/LED.h"
#include "Hardware/Vibrato.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/Beeper/Beeper.h"
#include "Storage/Storage.h"
#include "Display/Display.h"
#include "Modules/PMS150G/PMS150G.h"
#include "Hardware/Power.h"


// Ammyy      95 243 113     32766

/*
*   1. Уход в сон через некоторое время
*   2. Пробуждение по нажатии кнопки
*/


static void Update();

static void SaveSettings();


int main()
{
    HAL::Init();

    gset.Load();

    PCF8563::Init();

    Display::Init();

//    Power::Init();

    Timer::Init();

    Keyboard::Init();

    Menu::Init();

    LED::Init();

    Vibrato::Init();

    PAN3060::InitFull();

    Beeper::Init();

    Storage::Init();

    PMS150G::Init();

//    Power::Init();

    while(true)
    {
        Update();
    }
}


void Update()
{
    bool keyboard_more_time = Keyboard::ToMoreTime();
    int num_sources = Source::GetCountReceived();
    bool is_alarmed = PCF8563::IsAlarmed();

    if (keyboard_more_time && num_sources == 0 && !is_alarmed)
    {
        ModeClock::Set(ModeClock::Sleep);
    }

    ModeClock::LeaveSleepMode();

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

//    Power::Update();
}


void SaveSettings()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 5000)
    {
        meter.Reset();

        gset.Save();
    }
}
