// 2024/03/18 15:45:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Player/Player.h"
#include "Player/Beeper.h"
#include "Hardware/Timer.h"
#include <cmath>


namespace Player
{
    static uint8 NextSample(float frequency);

    static float k = 0.5f;

    static float frequency = 5274.0f;

    static uint prev_time = 0;

    namespace Watch
    {
        static uint time_ms = 0;

        static float CurrentTime()
        {
            if (time_ms != TIME_MS)
            {
                time_ms = TIME_MS;
                TimerUS::Reset();
            }

            return (float)time_ms / 1000.f + (float)TimerUS::ElaplsedTime() * 1e-6f;
        }
    }
}


void Player::Init()
{
    Beeper::Init();
}


void Player::CallbackOnTimer()
{
    Beeper::CallbackOnOutputSample(NextSample(frequency));

    if (TIME_MS > prev_time + 1000)
    {
        prev_time = TIME_MS;

        frequency *= k;

        if (frequency > 6000.0f)
        {
            k = 0.5f;
        }
        else if (frequency < 60.0f)
        {
            k = 2.0f;
        }
    }
}


uint8 Player::NextSample(float freq)
{
    return (uint8)(125.0f + 125.0f * std::sinf(2.0f * 3.1415926f * freq * Watch::CurrentTime()));
}
