#include "defines.h"
#include "gd32e23x_it.h"
#include "systick.h"
#include "Keyboard/Keyboard.h"
#include "Hardware/Timer.h"
#include "Modules/Beeper/Beeper.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modules/PAN3060/PAN3060.h"


namespace Keyboard
{
    int ms_for_disable = TIME_BLANK_DISPLAY;
}


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

    Keyboard::ms_for_disable--;

    delay_decrement();
}


void EXTI0_1_IRQHandler(void)
{
    if (SET == exti_interrupt_flag_get(EXTI_0))
    {
        Keyboard::CallbackFromInterrupt(Key::Cancel);

        exti_interrupt_flag_clear(EXTI_0);
    }

    if (SET == exti_interrupt_flag_get(EXTI_1))
    {
        Keyboard::CallbackFromInterrupt(Key::Down);

        exti_interrupt_flag_clear(EXTI_1);
    }
}

void EXTI2_3_IRQHandler(void)
{
    if (SET == exti_interrupt_flag_get(EXTI_2))
    {
        Keyboard::CallbackFromInterrupt(Key::Menu);

        exti_interrupt_flag_clear(EXTI_2);
    }
}


void EXTI4_15_IRQHandler(void)
{
    // Получено прерывание от приёмника
    if (SET == exti_interrupt_flag_get(EXTI_8))
    {
        exti_interrupt_flag_clear(EXTI_8);

        PAN3060::CallbackOnIRQ();
    }

    if (SET == exti_interrupt_flag_get(EXTI_7))
    {
        Keyboard::CallbackFromInterrupt(Key::Up);

        exti_interrupt_flag_clear(EXTI_7);
    }
}


void TIMER2_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH2))
    {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH2);

        Beeper::CallbackOnTimer();
    }
}


#ifdef __cplusplus
}
#endif
