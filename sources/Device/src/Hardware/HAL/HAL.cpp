// 2024/03/01 22:24:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Utils/Math.h"
#include <cstring>
#include "system.h"


namespace HAL
{
    // Сделать все порты выходами и записать в них ноль
    void AllPinsToOutput();

    static uint GetUID();
}


void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA); 
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOF);

//    AllPinsToOutput();

    rcu_periph_clock_enable(RCU_SPI0);          // Дислпей
    rcu_periph_clock_enable(RCU_SPI1);          // PAN3060

    rcu_periph_clock_enable(RCU_TIMER2);        // Для звука

#ifdef MODEL7735
    rcu_periph_clock_enable(RCU_CFGCMP);
    nvic_irq_enable(TIMER2_IRQn, 0);            // Для звука
#endif

#ifdef MODEL7789
    rcu_periph_clock_enable(RCU_AF);
    nvic_irq_enable(TIMER2_IRQn, 0, 0);         // Для звука
    rcu_periph_clock_enable(RCU_DMA0);          // Для записи в дисплей
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


void HAL::AllPinsToOutput()
{
#define MASK_PINS_A (GPIO_PIN_ALL & ~(GPIO_PIN_13 | GPIO_PIN_14))

#ifdef MODEL7735

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, MASK_PINS_A);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, MASK_PINS_A);
    GPIO_BC(GPIOA) = MASK_PINS_A;

    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_ALL);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
    GPIO_BC(GPIOB) = GPIO_PIN_ALL;

    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_ALL);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
    GPIO_BC(GPIOF) = GPIO_PIN_ALL;
#endif

#ifdef MODEL7789

    // PIN13, PIN14 - Отладчик
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MASK_PINS_A);
    GPIO_BC(GPIOA) = MASK_PINS_A;

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
    GPIO_BC(GPIOB) = GPIO_PIN_ALL;

    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
    GPIO_BC(GPIOF) = GPIO_PIN_ALL;

#endif
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


bool HAL::IsDebugBoard()
{
    volatile uint uid = GetUID();
    
    return uid == 0xAB4EF711 ||         // 7789
        uid == 0x3B279A51;              // 7735
}


uint HAL::GetUID()
{
    uint address =
#ifdef MODEL7789
        0x1FFFF7E8;
#else
        0x1FFFF7AC;
#endif

    uint *pointer = (uint *)address;

    uint8 buffer[12];

    std::memcpy(buffer, pointer, 12);

    return Math::CalculateCRC32(buffer, 12);
}

