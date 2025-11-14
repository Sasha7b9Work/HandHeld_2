// 2024/04/18 15:19:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modules/ST7735/ST7735.h"
#include "Hardware/HAL/systick.h"
#include "Keyboard/Keyboard.h"
#include "Settings/Source.h"
#include "Display/Display7735.h"
#include "Modules/PAN3060/PAN3060.h"
#include <gd32e23x.h>


ModeClock::E ModeClock::current = ModeClock::Low;


namespace HAL_CLOCK
{
    static void SetDeepSleep();
    static void SetLow();
    static void SetHi();

    static bool in_sleep_mode = false;
}


void ModeClock::Set(E v)
{
    if (v == ModeClock::DeepSleep)
    {
        if (!ModeClock::IsDeepSleep())
        {
            current = ModeClock::DeepSleep;

            HAL_CLOCK::in_sleep_mode = true;

            HAL_CLOCK::SetDeepSleep();
        }
    }
    else if (v == ModeClock::Low)
    {
        if (!ModeClock::IsLow())
        {
            current = ModeClock::Low;

            HAL_CLOCK::SetLow();
        }
    }
    else if (v == ModeClock::Hi)
    {
        if (!ModeClock::IsHi())
        {
            current = ModeClock::Hi;

            HAL_CLOCK::SetHi();
        }
    }
}


void ModeClock::LeaveDeepSleep()
{
    if (HAL_CLOCK::in_sleep_mode)
    {
        HAL_CLOCK::in_sleep_mode = false;

        ModeClock::Set(Source::ExistReceived() ? ModeClock::Hi : ModeClock::Low);
    }
}


#define RCU_MODIFY(__delay)     do{                                     \
                                    volatile uint32_t i;                \
                                    if(0 != __delay){                   \
                                        RCU_CFG0 |= RCU_AHB_CKSYS_DIV2; \
                                        for(i=0; i<__delay; i++){       \
                                        }                               \
                                        RCU_CFG0 |= RCU_AHB_CKSYS_DIV4; \
                                        for(i=0; i<__delay; i++){       \
                                        }                               \
                                    }                                   \
                                }while(0)



void HAL_CLOCK::SetDeepSleep()
{
    Display::PrepareToSleep();

    PAN3060::PrepareToSleep();

//    HAL::DeInit();

    rcu_periph_clock_enable(RCU_PMU);

    RCU_CTL0 &= ~RCU_CTL0_PLLEN;

    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, WFI_CMD);
}


void HAL_CLOCK::SetLow()
{
    SystemCoreClock = 3250000;

    /* enable IRC8M */
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;
    while (0U == (RCU_CTL0 & RCU_CTL0_IRC8MSTB)) {
    }

#ifndef WIN32
    RCU_MODIFY(0x80);
#endif
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CTL0 &= ~(RCU_CTL0_HXTALEN | RCU_CTL0_CKMEN | RCU_CTL0_PLLEN | RCU_CTL0_HXTALBPS);
    /* reset RCU */
    RCU_CFG0 &= ~(RCU_CFG0_SCS | RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC | \
        RCU_CFG0_ADCPSC | RCU_CFG0_CKOUTSEL | RCU_CFG0_CKOUTDIV | RCU_CFG0_PLLDV);
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4 | RCU_CFG0_PLLDV);
    RCU_CFG1 &= ~(RCU_CFG1_PREDV);
    RCU_CFG2 &= ~(RCU_CFG2_USART0SEL | RCU_CFG2_ADCSEL);
    RCU_CFG2 &= ~RCU_CFG2_IRC28MDIV;
    RCU_CFG2 &= ~RCU_CFG2_ADCPSC2;
    RCU_CTL1 &= ~RCU_CTL1_IRC28MEN;
    RCU_INT = 0x00000000U;

    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable HXTAL */
    RCU_CTL0 |= RCU_CTL0_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_HXTALSTB);
    } while ((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));
    /* if fail */
    if (0U == (RCU_CTL0 & RCU_CTL0_HXTALSTB)) {
        while (1) {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_2;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* PLL = HXTAL * 3 = 78 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLDV);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL2);

    rcu_hxtal_prediv_config(RCU_PLL_PREDV16);
    RCU_CFG0 |= RCU_CFG0_PLLDV;         // Должно быть то же, что и в PREDV2[0]

    /* enable PLL */
    RCU_CTL0 |= RCU_CTL0_PLLEN;

    /* wait until PLL is stable */
    while (0U == (RCU_CTL0 & RCU_CTL0_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }

    systick_config();

    HAL::Init();
}


void HAL_CLOCK::SetHi()
{
    SystemCoreClock = 78000000;

    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable IRC8M */
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;
    while (0U == (RCU_CTL0 & RCU_CTL0_IRC8MSTB)) {
    }

#ifndef WIN32
    RCU_MODIFY(0x80);
#endif
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CTL0 &= ~(RCU_CTL0_HXTALEN | RCU_CTL0_CKMEN | RCU_CTL0_PLLEN | RCU_CTL0_HXTALBPS);
    /* reset RCU */
    RCU_CFG0 &= ~(RCU_CFG0_SCS | RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC | \
        RCU_CFG0_ADCPSC | RCU_CFG0_CKOUTSEL | RCU_CFG0_CKOUTDIV | RCU_CFG0_PLLDV);
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4 | RCU_CFG0_PLLDV);
    RCU_CFG1 &= ~(RCU_CFG1_PREDV);
    RCU_CFG2 &= ~(RCU_CFG2_USART0SEL | RCU_CFG2_ADCSEL);
    RCU_CFG2 &= ~RCU_CFG2_IRC28MDIV;
    RCU_CFG2 &= ~RCU_CFG2_ADCPSC2;
    RCU_CTL1 &= ~RCU_CTL1_IRC28MEN;
    RCU_INT = 0x00000000U;


    /* enable HXTAL */
    RCU_CTL0 |= RCU_CTL0_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_HXTALSTB);
    } while ((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));
    /* if fail */
    if (0U == (RCU_CTL0 & RCU_CTL0_HXTALSTB)) {
        while (1) {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_2;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* PLL = HXTAL * 3 = 78 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLDV);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL3);

    /* enable PLL */
    RCU_CTL0 |= RCU_CTL0_PLLEN;

    /* wait until PLL is stable */
    while (0U == (RCU_CTL0 & RCU_CTL0_PLLSTB)) {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }

    systick_config();

    HAL::Init();
}
