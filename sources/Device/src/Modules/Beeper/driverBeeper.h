// 2024/03/03 17:06:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Beeper
{
    namespace Driver
    {
        void Init();

        void StartFrequency(float frequency, uint8 volume);

        void Stop();

        // Вывод отсчёта
        void CallbackOnOutputSample(uint8);
    }
}
