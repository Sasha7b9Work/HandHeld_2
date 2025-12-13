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
    
    int counter = 10;

    while (!PAN3060::InProcessUpgrade())        // Ждём, когда начнётся процесс обновления
    {
        if(counter-- < 0)
        {
            break;
        }
    }

    while (PAN3060::InProcessUpgrade())         // Если после выхода из предыдущего цикла PAN3060 не вошёл в режим обновления, то цикл выполняться не будет
    {
        Display::Update();
    }
}
