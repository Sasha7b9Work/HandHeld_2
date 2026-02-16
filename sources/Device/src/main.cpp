// 2024/03/01 22:30:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by// 
#include "defines.h"
#include "Device.h"


// Ammyy      95 243 113     32766

/*
*   1. Уход в сон через некоторое время
*   2. Пробуждение по нажатии кнопки
*/


int main()
{
    Device::Init();

    while (true)
    {
        Device::Update();
    }
}
