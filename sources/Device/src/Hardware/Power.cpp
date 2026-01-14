// 2024/03/20 23:04:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Power.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>


static PinOut pinCHRG(GPIOB, GPIO_PIN_3);


void Power::Init()
{
//    pinCHRG.Init();
//
//    pinCHRG.ToLow();
}


void Power::Draw()
{

}
