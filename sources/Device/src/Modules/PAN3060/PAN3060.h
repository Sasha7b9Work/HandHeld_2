// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


namespace PAN3060
{
    // Полная инициализация - производится при включении
    void InitFull();

    // Инициализировать вывести из режима сна на ожидание сигнала в течение 90 мс
    void InitOn90ms();

    void Update();

    // Вызывается по прерыванию на приём данных
    void CallbackOnIRQ();

    // Вызывается по прерыванию от сопроцессора PMS150G
    void CallbackOnPMS150();
}
