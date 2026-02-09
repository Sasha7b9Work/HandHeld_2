#include "defines.h"
#include "gd32e23x_it.h"
#include "systick.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Hardware/Timer.h"


#ifdef __cplusplus
extern "C" {
#endif


/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
#ifdef GD32E230
    if(SET == syscfg_flag_get(SYSCFG_SRAM_PCEF))
#endif
#ifdef GD32F303
    if(SET == syscfg_flag_get(SYSCFG_FLAG_SRAM_PCE))
#endif
		{
				//SRAM parity check error
				__NOP();
    } else {
        /* if NMI exception occurs, go to infinite loop */
        /* HXTAL clock monitor NMI error or NMI pin error */
        while(1) {
        }
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    timer_counter++;

    delay_decrement();
}


void EXTI4_15_IRQHandler(void)
{
    // Получено прерывание от приёмника
    if (SET == exti_interrupt_flag_get(EXTI_8))
    {
        exti_interrupt_flag_clear(EXTI_8);

        PAN3060::CallbackOnIRQ();
    }
}

#ifdef __cplusplus
}
#endif
