// 2025/11/24 09:42:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Upgrader.h"
#include "Modules/LED/LED.h"
#include "Hardware/Timer.h"
#include "Modules/PAN3060/PAN3060.h"


namespace Upgrader
{
    static void NotifyBeginUpgrade();
}


void Upgrader::Run()
{
    PAN3060::Init();

    LED::Init();

    TimeMeterMS meter;

    while (meter.ElapsedTime() < 3000)
    {
        PAN3060::Update();

        if (PAN3060::InProcessUpgrade())
        {
            NotifyBeginUpgrade();
            break;
        }
    }

    if (PAN3060::InProcessUpgrade())
    {
        while (PAN3060::InProcessUpgrade())
        {
            PAN3060::Update();
        }
    }
    else
    {

    }
}


void Upgrader::NotifyBeginUpgrade()
{
    LED::Enable(ColorLED::Red);

    Timer::Delay(500);

    LED::Enable(ColorLED::Green);

    Timer::Delay(500);

    LED::Enable(ColorLED::Blue);

    Timer::Delay(500);

    LED::Disable();
}
