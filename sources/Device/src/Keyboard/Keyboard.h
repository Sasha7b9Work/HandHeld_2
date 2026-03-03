// 2024/03/01 22:58:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "system.h"


struct Key
{
    enum E
    {
        Menu,
        Cancel,
        Up,
        Down,
        Count
    };
};


struct ActionType
{
    enum E
    {
        Down,
        Up,
        Count
    };
};


struct Action
{
    Key::E        key;
    ActionType::E type;

    bool IsRelease() const { return type == ActionType::Up; }
};


namespace Keyboard
{
    void Init();

    void Update();

    void CallbackFromInterrupt(Key::E);

    bool IsDown(Key::E);

    bool GetNextAction(Action &);

    // Есть ли невыполненные действия
    bool ActionExist();

    // Прошло много времени после последнего нажатия кнопки
    bool ToMoreTime();

    // Столько миллисекунд осталось до момента, когда дисплей можно выключать.
    extern int ms_for_disable;
}
