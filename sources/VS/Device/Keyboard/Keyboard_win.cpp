// 2024/03/02 13:18:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Keyboard/Keyboard.h"
#include <cstring>


namespace Keyboard
{
    int ms_for_disable = 0;

    static const int MAX_ACTIONS = 10;

    static Action actions[MAX_ACTIONS];

    static int num_actions = 0;

    void AppendAction(const Action &action)
    {
        actions[num_actions] = action;
        num_actions++;
    }
}


void Keyboard::Init()
{

}


bool Keyboard::IsDown(Key::E)
{
    return false;
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
        std::memmove(actions, actions + 1, sizeof(Action) * (num_actions - 1));
        num_actions--;
    }

    return true;
}


bool Keyboard::ToMoreTime()
{
    return false;
}
