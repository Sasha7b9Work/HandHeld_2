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
    rcu_periph_clock_enable(RCU_SPI1);

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


#ifdef MODEL7735

void syscfg_exti_line_clear(uint8 exti_pin)
{
    uint32_t clear_exti_mask = ~((uint32_t)EXTI_SS_MASK << (EXTI_SS_MSTEP(exti_pin)));

    switch (exti_pin / EXTI_SS_JSTEP)
    {
    case EXTISS0:
        /* clear EXTI source line(0..3) */
        SYSCFG_EXTISS0 &= clear_exti_mask;
        break;
    case EXTISS1:
        /* clear EXTI soure line(4..7) */
        SYSCFG_EXTISS1 &= clear_exti_mask;
        break;
    case EXTISS2:
        /* clear EXTI soure line(8..11) */
        SYSCFG_EXTISS2 &= clear_exti_mask;
        break;
    case EXTISS3:
        /* clear EXTI soure line(12..15) */
        SYSCFG_EXTISS3 &= clear_exti_mask;
        break;
    default:
        break;
    }
}

#endif

