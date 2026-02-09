// 2025/11/17 21:37:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#ifdef GD32E230
	#include <gd32e23x.h>
#endif
#ifdef GD32F303
	#include <gd32f30x.h>
#endif


void HAL::Init()
{
    systick_config();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);

		rcu_periph_clock_enable(RCU_SPI0);
		rcu_periph_clock_enable(RCU_DMA0);
	
#ifdef GD32E230
    rcu_periph_clock_enable(RCU_CFGCMP);
#endif
#ifdef GD32F303
		rcu_periph_clock_enable(RCU_AF);
#endif
}
