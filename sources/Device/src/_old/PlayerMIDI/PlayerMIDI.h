#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "PlayerMIDI/PlayerConfigMIDI.h"


struct TypeMelody
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

struct MelodyMIDI
{
    const uint8 *pStream1;
    const uint8 *pStream2;
};


namespace PlayerMIDI
{
    void Play(TypeMelody::E);

    void Init();

    // Is player currently playing ?
    bool IsPlaying();

    // Wait untill player finish playing
    // If not playing currently, returns immediatelly.
    void WaitFinish();

    // Остановить воспроизводимую в данный момент мелодию. Обратный вызов Player_Finished() вызывается, если игра
    // воспроизводилась. Если мелодия не звучит - звонок полностью игнорируется.
    void Stop();

    // Функция события таймера должна вызываться пользователем с фиксированной частотой HXMIDIPLAYER_SAMPLING_RATE
    void CallbackOnTimer();
}
