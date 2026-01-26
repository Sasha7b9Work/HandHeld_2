#include "defines.h"
#include "gd32f30x_it.h"
#include "systick.h"
#include "Keyboard/Keyboard.h"
#include "Hardware/Timer.h"
#include "Modules/Beeper/Beeper.h"
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


#define CALLBACK_ON_KEY(ext, key)               \
    if (SET == exti_interrupt_flag_get(ext))    \
    {                                           \
        Keyboard::CallbackFromInterrupt(key);   \
        exti_interrupt_flag_clear(ext);         \
    }


void EXTI2_3_IRQHandler(void)
{
#ifdef BOARD_NEW

    CALLBACK_ON_KEY(EXTI_3, Key::Menu)

#else

    /*
        bool PinIn::IsHi() const
        {
            return gpio_input_bit_get(port, pin) != RESET;
        }
    */

    static bool prev_cancel = false;
    static bool prev_menu = false;

    {
        bool cancel = gpio_input_bit_get(GPIOB, GPIO_PIN_3) != RESET;

        if (cancel != prev_cancel)
        {
            prev_cancel = cancel;

            Keyboard::CallbackFromInterrupt(Key::Cancel);

            exti_interrupt_flag_clear(EXTI_3);
        }
    }

    {
        bool menu = gpio_input_bit_get(GPIOA, GPIO_PIN_3) != RESET;

        if (menu != prev_menu)
        {
            prev_menu = menu;

            Keyboard::CallbackFromInterrupt(Key::Menu);

            exti_interrupt_flag_clear(EXTI_3);
        }
    }

#endif
}


void EXTI4_15_IRQHandler(void)
{
    // Получено прерывание от приёмника
    if (SET == exti_interrupt_flag_get(EXTI_8))
    {
        PAN3060::CallbackOnIRQ();

        exti_interrupt_flag_clear(EXTI_8);
    }

    if (SET == exti_interrupt_flag_get(EXTI_13))
    {
        exti_interrupt_flag_clear(EXTI_13);

        // Хотя это прерывание случается на PC13 от PMS150G, но вызываем приёмник - пора слушать эфир
        PAN3060::CallbackOnWakeUp();
    }

    CALLBACK_ON_KEY(EXTI_5, Key::Down)

    CALLBACK_ON_KEY(EXTI_12, Key::Up)

#ifdef BOARD_NEW

    CALLBACK_ON_KEY(EXTI_15, Key::Cancel)

#endif
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
