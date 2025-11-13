// 2024/04/01 10:52:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/LED/driverLED.h"
#include <gd32e23x.h>


void LED::Driver::Init()
{
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
}


void LED::Driver::On()
{
    gpio_bit_set(GPIOA, GPIO_PIN_9);
    gpio_bit_set(GPIOA, GPIO_PIN_10);
    gpio_bit_set(GPIOA, GPIO_PIN_11);
}


void LED::Driver::Off()
{
    gpio_bit_reset(GPIOA, GPIO_PIN_9);
    gpio_bit_reset(GPIOA, GPIO_PIN_10);
    gpio_bit_reset(GPIOA, GPIO_PIN_11);
}


bool LED::Driver::IsFired()
{
    return gpio_input_bit_get(GPIOA, GPIO_PIN_2) != RESET;
}
