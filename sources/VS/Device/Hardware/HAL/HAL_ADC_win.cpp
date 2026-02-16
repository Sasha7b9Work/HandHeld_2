// 2024/03/20 19:29:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


namespace HAL_ADC
{
    // Эта функция не вынесена в общий доступ потому, что её нельзя вызывать когда попало - надо вызывать только когда потребление не
    // сильно высоко, чтобы ёмкость замерить более-менее адекватно
    float GetVoltage(bool force);
}


float HAL_ADC::GetVoltage(bool)
{
    return 0.0f;
}
