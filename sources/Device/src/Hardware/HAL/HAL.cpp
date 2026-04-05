// 2024/03/01 22:24:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Utils/Math.h"
#include "Modules/ST7735_89/ST7735_89.h"
#include <cstring>
#include "system.h"


namespace HAL
{
    // Сделать все порты выходами и записать в них ноль
    static void AllPinsDisable();

    static void AllPinsEnable();

    static uint GetUID();
}

void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA); 
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOF);

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

    AllPinsEnable();
}


void HAL::DeInit()
{
    AllPinsDisable();

    rcu_periph_clock_disable(RCU_GPIOA);
//    rcu_periph_clock_disable(RCU_GPIOB);        // Не выходит по сигналу приёмника
    rcu_periph_clock_disable(RCU_GPIOF);

#ifdef MODEL7735
    rcu_periph_clock_disable(RCU_CFGCMP);
    rcu_periph_clock_disable(RCU_ADC);
#endif

#ifdef MODEL7789
    rcu_periph_clock_disable(RCU_ADC0);
#endif

    rcu_periph_clock_disable(RCU_TIMER2);
    rcu_periph_clock_disable(RCU_SPI0);
//    rcu_periph_clock_disable(RCU_SPI1);       // Если отключить, то не выходит из сна по прёмнику и кнопкам
}


void HAL::AllPinsEnable()
{
    ST7735_89::EnablePins();

#ifdef MODEL7735
#endif

#ifdef MODEL7789
#endif
}


void HAL::AllPinsDisable()
{
#ifdef MODEL7735

    const uint mask_a =
        GPIO_PIN_0 |    // I2C_SCL  часы
        GPIO_PIN_1 |    // I2C_SDA  часы
        GPIO_PIN_2 |    // ADC
//        GPIO_PIN_3 |    // SW_RGHT
//        GPIO_PIN_4 |    // TFT_BKG
//        GPIO_PIN_5 |    // TFT_SCL
        GPIO_PIN_6 |    // n/a
//        GPIO_PIN_7 |    // TFT_SDA
//        GPIO_PIN_8 |    // IRQ
        GPIO_PIN_9 |    // LEDR
        GPIO_PIN_10 |   // LEDG
        GPIO_PIN_11;   // LEDB
//        GPIO_PIN_12 |   // SW_UP
//        GPIO_PIN_13 |   // SWDIO
//        GPIO_PIN_14 |   // SWCLK
//        GPIO_PIN_15;    // SWLEFT

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, mask_a);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, mask_a);
    GPIO_BC(GPIOA) = mask_a;

    const uint mask_b =
        GPIO_PIN_0 |    // SOUND
        GPIO_PIN_1 |    // MUTE SOUND
//        GPIO_PIN_2 |    // TFT_ON
//        GPIO_PIN_3 |    // CHRG_CPU
        GPIO_PIN_4 |    // VIBRO
//        GPIO_PIN_5 |    // SW_DWN
        GPIO_PIN_6 |    // n/a
        GPIO_PIN_7 |    // n/a
        GPIO_PIN_8 |    // n/a
        GPIO_PIN_9;    // n/a
//        GPIO_PIN_10 |   // TFT_RES
//        GPIO_PIN_11;    // TFT_RS
//        GPIO_PIN_12 |   // NSS      PA3060
//        GPIO_PIN_13 |   // SCK      PA3060
//        GPIO_PIN_14 |   // MISO     PA3060
//        GPIO_PIN_15;    // MOSI     PA3060

    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, mask_b);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, mask_b);
    GPIO_BC(GPIOB) = mask_b;

    const uint mask_f =
        GPIO_PIN_6 |    // n/a
        GPIO_PIN_7;     // n/a

    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, mask_f);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, mask_f);
    GPIO_BC(GPIOB) = mask_f;

#endif

#ifdef MODEL7789

//    // PIN13, PIN14 - Отладчик
//    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, MASK_PINS_A);
//    GPIO_BC(GPIOA) = MASK_PINS_A;

//    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
//    GPIO_BC(GPIOB) = GPIO_PIN_ALL;

//    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_ALL);
//    GPIO_BC(GPIOF) = GPIO_PIN_ALL;

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
    
    return uid == 0xAB4EF711 ||     // 7789
        uid == 0x3B279A51 ||        // 7735
        uid == 0x1203D5E3 ||        // 7735
        uid == 0x5D9A3324;          // 7735
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

