// 2024/03/18 15:45:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Settings/Settings.h"


namespace Beeper
{
    void Init();

    void Update();

    void Play(Melody::E, uint8);

    void Stop();

    void CallbackOnTimer();

    bool IsRunning();

    uint TimeAfterStop();
}
