// 2022/11/23 14:55:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Utils/String.h"
#include "stm_includes.h"


namespace Device
{
    static void JumpToMainApplication();

    // Есть ли новая прошивка для обновления
    static bool ExistFirmwareInROM();

    static void UpgradeFirmware();
}


void Device::Init()
{
}


void Device::Update()
{
    if(ExistFirmwareInROM())
    {
        UpgradeFirmware();
    }
    
    JumpToMainApplication();
}


bool Device::ExistFirmwareInROM()
{
    return false;
}


void Device::UpgradeFirmware()
{
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
