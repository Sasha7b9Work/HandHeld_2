// 2024/03/01 23:01:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Keyboard/Keyboard.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include <cstring>
#include <gd32e23x.h>


namespace Keyboard
{
    struct Button
    {
        Button(uint _port, uint16 _pin, uint8 _irqn, uint8 _exti_source_port, uint8 _exti_source_pin, exti_line_enum _exti) :
            port(_port), pin(_pin), irqn(_irqn), exti_source_port(_exti_source_port), exti_source_pin(_exti_source_pin), exti(_exti)
        { }

        void Init()
        {
            gpio_mode_set(port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin);
            nvic_irq_enable(irqn, 2);
            syscfg_exti_line_config(exti_source_port, exti_source_pin);
            exti_init(exti, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
            exti_interrupt_flag_clear(exti);
        }

        bool IsDown()
        {
            return (gpio_input_bit_get(port, pin) == RESET);
        }
        bool IsUp()
        {
            return !IsDown();
        }

    private:
        uint   port;
        uint16 pin;
        uint8  irqn;
        uint8  exti_source_port;
        uint8  exti_source_pin;
        exti_line_enum exti;
    };


#ifdef TYPE_1602
    static Button btnMenu(GPIOB, GPIO_PIN_6, EXTI4_15_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN6, EXTI_6); 
    static Button btnCancel(GPIOB, GPIO_PIN_4, EXTI4_15_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN4, EXTI_4);
    static Button btnUp(GPIOB, GPIO_PIN_5, EXTI4_15_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN5, EXTI_5);
    static Button btnDown(GPIOB, GPIO_PIN_15, EXTI4_15_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN15, EXTI_15);
#else
    static Button btnMenu(GPIOB, GPIO_PIN_2, EXTI2_3_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN2, EXTI_2);
    static Button btnCancel(GPIOB, GPIO_PIN_0, EXTI0_1_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN0, EXTI_0);
    static Button btnUp(GPIOA, GPIO_PIN_7, EXTI4_15_IRQn, EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN7, EXTI_7);
    static Button btnDown(GPIOB, GPIO_PIN_1, EXTI0_1_IRQn, EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN1, EXTI_1);
#endif

    struct ButtonStruct
    {
        Button *button;
        bool prev_down;
        uint prev_time;     // ����� ������������ ������ ������ ���� ������ ����� ������������ ������� - �����������
    };

    static ButtonStruct buttons[Key::Count] =
    {
        {&btnMenu, false, 0},
        {&btnCancel, false, 0},
        {&btnUp, false, 0},
        {&btnDown, false, 0}
    };

    static const int MAX_ACTIONS = 10;

    static Action actions[MAX_ACTIONS];

    static int num_actions = 0;

    static void AppendAction(const Action &action)
    {
        actions[num_actions] = action;
        num_actions++;
    }
}


void Keyboard::Init()
{
    btnMenu.Init();
    btnCancel.Init();
    btnUp.Init();
    btnDown.Init();
}


void Keyboard::CallbackFromInterrupt(Key::E key)
{
    static TimeMeterMS meter;

    if (ms_for_disable < 0)             // ����� �� ������� ������
    {
        meter.Reset();
    }

    ms_for_disable = TIME_BLANK_DISPLAY;

    if (!Source::GetCountReceived())
    {
        if (meter.ElapsedTime() < 200)
        {
            return;
        }
    }

    if (key != Key::Count)
    {
        uint time = TIME_MS;

        if (time - buttons[key].prev_time > 250)
        {
            bool is_down = buttons[key].button->IsDown();       // ���� ���� �� ����� ��� ���� - �� ����, ��� ����� ���� �����

//          if (buttons[key].prev_down != is_down)
//          {
                if (is_down)
                {
                    AppendAction({ key , ActionType::Up });
                }

                buttons[key].prev_time = time;
//              buttons[key].prev_down = is_down;
//          }
        }
    }
}


bool Keyboard::ToMoreTime()
{
    return ms_for_disable < 0;
}


bool Keyboard::IsDown(Key::E key)
{
    return buttons[key].button->IsDown();
}


bool Keyboard::GetNextAction(Action &action)
{
    if (num_actions == 0)
    {
        return false;
    }

    action = actions[0];

    if (num_actions == 1)
    {
        num_actions = 0;
    }
    else
    {
        std::memmove(actions, actions + 1, (uint)(sizeof(Action) * (uint)(num_actions - 1)));

        num_actions--;
    }

    return true;
}
