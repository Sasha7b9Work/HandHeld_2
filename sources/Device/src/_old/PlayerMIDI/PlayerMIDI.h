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

    // ���������� ��������������� � ������ ������ �������. �������� ����� Player_Finished() ����������, ���� ����
    // ����������������. ���� ������� �� ������ - ������ ��������� ������������.
    void Stop();

    // ������� ������� ������� ������ ���������� ������������� � ������������� �������� HXMIDIPLAYER_SAMPLING_RATE
    void CallbackOnTimer();
}
