// 2024/03/01 22:24:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "system.h"


void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA); 
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_periph_clock_enable(RCU_SPI0);

    rcu_periph_clock_enable(RCU_TIMER2);        // Для звука

#ifdef MODEL7735
    rcu_periph_clock_enable(RCU_CFGCMP);
    nvic_irq_enable(TIMER2_IRQn, 0);            // Для звука
#endif

#ifdef MODEL7789
    rcu_periph_clock_enable(RCU_AF);
    nvic_irq_enable(TIMER2_IRQn, 0, 0);        // Для звука
#endif

    HAL_ADC::Init();

    HAL_I2C::Init();
}


void HAL::DeInit()
{
#ifdef MODEL7735
    rcu_periph_clock_disable(RCU_CFGCMP);
    rcu_periph_clock_disable(RCU_ADC);
#endif

    rcu_periph_clock_disable(RCU_TIMER2);
}

