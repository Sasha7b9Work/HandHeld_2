// 2022/11/23 14:55:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Hardware/Keyboard.h"
#include "Modules/LED/LED.h"
#include "Hardware/Timer.h"
#include "Upgrader.h"


namespace Device
{
    static void JumpToMainApplication();

    static bool NeedUpgrade();
}


void Device::Init()
{
    HAL::Init();

    PAN3060::Init();

    LED::Init();

    Keyboard::Init();
}


void Device::Update()
{
//    if (NeedUpgrade())
//    {
//        Upgrader::Run();
//    }

    JumpToMainApplication();
}


bool Device::NeedUpgrade()
{
    return Keyboard::NeedUpgrade();
}


void Device::JumpToMainApplication()
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
