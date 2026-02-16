// 2024/03/06 14:35:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "GUI/Controls/PainterMelody.h"
#include "GUI/Frame.h"


namespace Beeper
{
    namespace Driver
    {
        void Init();

        void StartFrequency(float frequency, uint8 vol, bool first);

        void Stop();
    }
}


void Beeper::Driver::Init()
{
}


void Beeper::Driver::StartFrequency(float /*frequency*/, uint8 /*vol*/, bool /*first*/)
{
}


void Beeper::Driver::Stop()
{
}
