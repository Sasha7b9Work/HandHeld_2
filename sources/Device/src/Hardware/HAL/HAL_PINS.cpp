// 2024/03/01 22:10:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>

const uint I2C_ADDR = I2C1;

PinIn pinSW_LEFT(GPIOB, GPIO_PIN_0);        // PB0  18 
PinIn pinSW_DOWN(GPIOB, GPIO_PIN_1);        // PB1  19
PinIn pinSW_RIGHT(GPIOB, GPIO_PIN_2);       // PB2  20
PinIn pinSW_UP(GPIOA, GPIO_PIN_7);          // PA7  17

PinIn pinDOUT(GPIOB, GPIO_PIN_12);          // PB12 25
PinIn pinSCK(GPIOB, GPIO_PIN_13);           // PB13 26

PinIn  pinPWR_CTRL(GPIOA, GPIO_PIN_0);      // PA0  10
PinOut pinADC(GPIOA, GPIO_PIN_1);           // PA1  11

PinOut pinLEDPWR(GPIOA, GPIO_PIN_2);        // PA2  12

PinOut pinPWR(GPIOA, GPIO_PIN_12);           // PA12 33


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
