// 2024/03/01 22:24:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32e23x.h>


void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA); 
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_periph_clock_enable(RCU_CFGCMP);

    rcu_periph_clock_enable(RCU_I2C1);

    rcu_periph_clock_enable(RCU_TIMER2);
    nvic_irq_enable(TIMER2_IRQn, 0);            // Таймер на приёмник

    HAL_I2C::Init();

    HAL_ADC::Init();

    HAL_TIM2::Init();
}


void HAL::DeInit()
{
    rcu_periph_clock_disable(RCU_CFGCMP);
    rcu_periph_clock_disable(RCU_I2C1);
    rcu_periph_clock_disable(RCU_ADC);
    rcu_periph_clock_disable(RCU_TIMER2);
}

