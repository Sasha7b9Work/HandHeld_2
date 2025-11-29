// 2025/11/24 09:42:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Upgrader.h"
#include "Hardware/Timer.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Display/Display7735.h"


void Upgrader::Run()
{
    PAN3060::Init();

    Display::Init();

    TimeMeterMS meter;

    while (!PAN3060::InProcessUpgrade())
    {
        Display::Update();
    }

    while (PAN3060::InProcessUpgrade())
    {
        Display::Update();
    }
}
