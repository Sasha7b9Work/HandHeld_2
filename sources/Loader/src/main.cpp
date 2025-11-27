// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Modules/LED/LED.h"
#include "Hardware/Keyboard.h"


#ifndef WIN32
    #if (__ARMCC_VERSION < 6180000) || (__ARMCC_VERSION >= 6190000)
        #error("Требуется компилятор версии 6.18. Прошивка, скомпилированная на другом компиляторе, может неправильно работать")
    #endif
#endif


static void JumpToMainApplication();

bool NeedUpgrade();


int main()
{
    HAL::Init();

    PAN3060::Init();

    LED::Init();

    Keyboard::Init();

    while (1)
    {
//    if (NeedUpgrade())
//    {
//        Upgrader::Run();
//    }

        JumpToMainApplication();
    }
}


bool NeedUpgrade()
{
    return Keyboard::NeedUpgrade();
}


void JumpToMainApplication()
{
#define APP_ADDRESS 0x8002000

#ifndef WIN32

    typedef void (*pFunction)(void);
    pFunction jump_to_app;
    uint32_t jump_address;

    __disable_irq();

    SCB->VTOR = APP_ADDRESS;

    __set_MSP(*(__IO uint *)APP_ADDRESS);

    jump_address = *(__IO uint32_t *)(APP_ADDRESS + 4);
    jump_to_app = (pFunction)jump_address;
    __enable_irq();

    jump_to_app();

#endif
}
