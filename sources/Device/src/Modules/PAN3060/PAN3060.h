// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


namespace PAN3060
{
    void Init();

    void Update();

    // Идут ли клоки
    bool IsEnabled();

    void PrepareToSleep();

    // Вызывается по прерыванию на приём данных
    void CallbackOnIRQ();

    // Вызывается по прерыванию от сопроцессора PMS150G
    void CallbackOnWakeUp();
}
