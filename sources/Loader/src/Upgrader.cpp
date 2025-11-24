// 2025/11/24 09:42:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Upgrader.h"
#include "Modules/LED/LED.h"
#include "Hardware/Timer.h"


namespace Upgrader
{
    static void NotifyBeginUpgrade();

    static bool ReceiveFirmwareFromPAN3060();
}


void Upgrader::Run()
{
    while (true)
    {
        NotifyBeginUpgrade();

        if (ReceiveFirmwareFromPAN3060())
        {
            break;
        }
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


bool Upgrader::ReceiveFirmwareFromPAN3060()
{
    return true;
}
