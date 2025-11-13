// 2024/03/18 15:45:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct TypeSound
{
    enum E
    {
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        _10,
        Count
    };
};


namespace Beeper
{
    void Init();

    void Update();

    void Play(TypeSound::E, uint8);

    void Stop();

    void CallbackOnTimer();

    bool IsRunning();
}
