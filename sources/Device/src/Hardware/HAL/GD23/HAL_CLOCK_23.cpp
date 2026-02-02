// 2024/04/18 15:19:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Display/Display.h"
#include <gd32e23x.h>


ModeClock::E ModeClock::current = ModeClock::Low;


namespace HAL_CLOCK
{
    static void SetSleepMode();
    static void SetLow();
    static void SetHi();

    static bool in_sleep_mode = false;
}


void ModeClock::Set(E v)
{
    if (v == ModeClock::Sleep)
    {
        if (!ModeClock::IsSleep())
        {
            current = ModeClock::Sleep;

            HAL_CLOCK::in_sleep_mode = true;

            HAL_CLOCK::SetSleepMode();
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


void ModeClock::LeaveSleepMode()
{
    if (HAL_CLOCK::in_sleep_mode)
    {
        HAL_CLOCK::in_sleep_mode = false;

        PAN3060::InitRF();

        PAN3060::Update();

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



void HAL_CLOCK::SetSleepMode()
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
    SystemCoreClock = 8000000;

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;

    /* select IRC8M as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_IRC8M;

    /* wait until IRC8M is selected as system clock */
    while (RCU_SCSS_IRC8M != (RCU_CFG0 & RCU_CFG0_SCSS))
    {
    }

    systick_config();

    HAL::Init();
}


void HAL_CLOCK::SetHi()
{
    SystemCoreClock = 72000000;

    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;

    /* enable IRC8M */
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;

    /* wait until IRC8M is stable or the startup time is longer than IRC8M_STARTUP_TIMEOUT */
    do
    {
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_IRC8MSTB);
    } while ((0U == stab_flag) && (IRC8M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if (0U == (RCU_CTL0 & RCU_CTL0_IRC8MSTB))
    {
        while (1)
        {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_2;

    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;
    /* PLL = (IRC8M/2) * 18 = 72 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF);
    RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2 | RCU_PLL_MUL18);

    /* enable PLL */
    RCU_CTL0 |= RCU_CTL0_PLLEN;

    /* wait until PLL is stable */
    while (0U == (RCU_CTL0 & RCU_CTL0_PLLSTB))
    {
    }

    /* select PLL as system clock */
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    RCU_CFG0 |= RCU_CKSYSSRC_PLL;

    /* wait until PLL is selected as system clock */
    while (RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS))
    {
    }

    systick_config();

    HAL::Init();
}
