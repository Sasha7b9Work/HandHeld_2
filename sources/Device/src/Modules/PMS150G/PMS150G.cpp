// 2025/12/18 17:21:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/PMS150G/PMS150G.h"
#include "system.h"


namespace PMS150G
{

}


void PMS150G::Init()
{
#ifdef MODEL7735

    // PWR_CTRL
    // Инициализируем пин клоков от приёмника на прерывание
    gpio_mode_set(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    nvic_irq_enable(EXTI4_15_IRQn, 2);
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_RISING);

#endif
}
