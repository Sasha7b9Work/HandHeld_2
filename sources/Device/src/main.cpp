// 2024/03/01 22:30:00 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by// 
#include "defines.h"
#include "Device.h"


int main()
{
    Device::Init();

    while (true)
    {
        Device::Update();
    }
}
