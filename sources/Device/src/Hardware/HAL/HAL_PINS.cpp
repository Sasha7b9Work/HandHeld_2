// 2024/03/01 22:10:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


PinOut pinSPI1_NSS(GPIOB, GPIO_PIN_12);


void Pin::Init()
{

}


void PinOut::Init()
{
#ifdef MODEL7735
    gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
    gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin);
#endif

#ifdef MODEL7789
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);
#endif
}


void PinIn::Init()
{
#ifdef MODEL7735
    gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
#endif

#ifdef MODEL7789
    gpio_init(port, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, pin);
#endif
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
