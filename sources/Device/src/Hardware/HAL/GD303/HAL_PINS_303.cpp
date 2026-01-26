// 2026/01/26 16:45:34 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


PinOut pinSPI1_NSS(GPIOB, GPIO_PIN_12);
PinOut pinSPI1_CLK(GPIOB, GPIO_PIN_13);
PinOut pinSPI1_MOSI(GPIOB, GPIO_PIN_15);
PinIn  pinSPI_MISO(GPIOB, GPIO_PIN_14);


void PinIn::Init()
{

}


bool PinIn::IsHi() const
{
    return false;
}


void PinOut::Init()
{

}


void PinOut::ToHi() const
{

}


void PinOut::ToLow() const
{

}
