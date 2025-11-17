// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"


int main()
{
#ifndef WIN32
    #if (__ARMCC_VERSION < 6180000) || (__ARMCC_VERSION >= 6190000)
        #error("Требуется компилятор версии 6.18. Прошивка, скомпилированная на другом компиляторе, может неправильно работать")
    #endif
#endif

    Device::Init();

    while (1)
    {
        Device::Update();
    }
}
