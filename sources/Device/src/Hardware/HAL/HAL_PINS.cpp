// 2024/03/01 22:10:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>


PinIn  pinPWR_CTRL(GPIOA, GPIO_PIN_0);          // PA0  10
PinOut pinPWR(GPIOA, GPIO_PIN_12);              // PA12 33

PinOut pinSPI1_NSS(GPIOB, GPIO_PIN_12);
PinOut pinSPI1_CLK(GPIOB, GPIO_PIN_13);
PinOut pinSPI1_MOSI(GPIOB, GPIO_PIN_14);
PinIn  pinSPI_MISO(GPIOB, GPIO_PIN_15);



void Pin::Init()
{

}


void PinOut::Init()
{
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinIn::Init()
{
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
}


bool PinIn::IsHi() const
{
    return gpio_input_bit_get(port, pin) != RESET;
}


void PinOut::ToLow() const
{
    gpio_bit_reset(port, pin);
}


void PinOut::ToHi() const
{
    gpio_bit_set(port, pin);
}


bool PinOut::IsHi() const
{
    return gpio_output_bit_get(port, pin) != RESET;
}
