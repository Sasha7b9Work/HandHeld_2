// 2024/03/01 22:30:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by// 
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Display/Display7735.h"
#include "Hardware/Timer.h"
#include "Keyboard/Keyboard.h"
#include "Menu/Menu.h"
#include "Hardware/Vibrato.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Storage/Storage.h"
#include "Hardware/Power.h"
#include <cstdlib>


// Ammyy      95 243 113     32766



void Update();

static void Update2();

static void UpdateLED();


int main()
{
    HAL::Init();

    Display::Init();

    //    Power::Init();

    Timer::Init();

    Keyboard::Init();

    Menu::Init();

    Vibrato::Init();

//    ModeClock::Set(ModeClock::Hi);

    PAN3060::Init();

    Storage::Init();
    
    PCF8563::Init();

    RTCDateTime time
    {
        (uint8)(std::rand() % 100),
        (uint8)((std::rand() % 10) + 1),
        (uint8)(std::rand() % 30),
        (uint8)(std::rand() % 12),
        (uint8)(std::rand() % 60),
        (uint8)(std::rand() % 60)
    };

    PCF8563::SetDateTime(&time);

    while (true)
    {
        Update2();
    }
}


void Update()
{
    bool recv_enabled = PAN3060::IsEnabled();
    bool keyboard_more_time = Keyboard::ToMoreTime();
    int num_sources = Source::GetCountReceived();
    bool is_alarmed = PCF8563::IsAlarmed();

    if (!recv_enabled && keyboard_more_time && num_sources == 0 && !is_alarmed)
    {
//        ModeClock::Set(ModeClock::DeepSleep);
    }

    ModeClock::LeaveDeepSleep();

    PCF8563::Update();

    Menu::Update();

    PAN3060::Update();

    if (Source::GetCountReceived() || !Keyboard::ToMoreTime())
    {
        Display::Update();
    }

    Vibrato::Update();

    Source::Update();

    Power::Update();
}


void Update2()
{
    PAN3060::Update();

    PCF8563::Update();

    UpdateLED();

    Display::UpdateDebug();
}


void UpdateLED()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 1000)
    {
        meter.Reset();

        static int color = 0;

        color++;

        color &= 0x07;
    }
}
