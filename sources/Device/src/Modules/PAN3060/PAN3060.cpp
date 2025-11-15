// 2024/03/01 22:46:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/PAN3060/PAN3060.h"
#ifdef ENABLE_EMULATOR
    #include "Modules/CMT2210AW/EmulatorReceiver.h"
#endif
#include <gd32e23x.h>
#include <cstring>


namespace PAN3060
{
    /*
    *   IRQ  - PA8
    *   NSS  - PB12     SPI0_NSS
    *   MISO - PB14     SIP0_MISO
    *   MOSI - PB15     SPI0_MOSI
    */

    static uint time_enable = 0;        // Время, когда начались клоки

    static bool need_start = false;
}


void PAN3060::Init()
{
//    pinDOUT.Init();

    // Инициализируем пин клоков от приёмника на прерывание
    gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN13);
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_13);
}


void PAN3060::Update()
{
    if (need_start)
    {
        need_start = false;

        HAL_TIM2::Init();

        time_enable = TIME_MS;
    }
}


void PAN3060::PrepareToSleep()
{
#ifdef WIN32
#else
    EXTI_PD = EXTI_13;
    EXTI_INTEN |= EXTI_13;
#endif
}


void PAN3060::CallbackOnIRQ()
{
#ifdef WIN32
#else
    EXTI_INTEN &= ~(uint)EXTI_13;
#endif
    need_start = true;
}


bool PAN3060::IsEnabled()
{
    bool result = TIME_MS - time_enable < 700;      // \todo здесь должно быть 610

    if (!result)
    {
        PrepareToSleep();
    }

    return result;
}


uint PAN3060::GetCode(Source::E source)
{
    static const uint packets[Source::Count] = { 0x7E9E, 0x7EA6, 0x7ED5, 0x7EB9, 0x7ECA, 0x7EED };

    return packets[source];
}
