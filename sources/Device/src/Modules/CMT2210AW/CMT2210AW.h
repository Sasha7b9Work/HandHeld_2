// 2024/03/01 22:46:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


namespace CMT2210AW
{
    void Init();

    void Update();

    // ���� �� �����
    bool IsEnabled();

    void PrepareToSleep();

    uint GetCode(Source::E);

    // ���������� �� ����� ���������� ����
    void CallbackOnBit();

    void CallbackOnClock();
}
