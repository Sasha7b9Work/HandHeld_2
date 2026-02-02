// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


namespace PAN3060
{
    // Полная инициализация - производится при включении
    void InitFull();

    // Инициализация непосредственно приёмника - производится после выхода из спящего режима
//    void InitRF();

    void Update();

    void PrepareToSleep();

    // Вызывается по прерыванию на приём данных
    void CallbackOnIRQ();

    // Вызывается по прерыванию от сопроцессора PMS150G
    void CallbackOnWakeUp();
}
