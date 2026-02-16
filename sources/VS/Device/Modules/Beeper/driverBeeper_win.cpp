// 2024/03/06 14:35:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Beeper/driverBeeper.h"
#include "GUI/Controls/PainterMelody.h"
#include "GUI/Frame.h"


void Beeper::Driver::Init()
{
    PainterMelody::Create(Frame::self, Painter::size_indicator);

    PainterMelody::self->SetPosition({ 550, 18 });
}


void Beeper::Driver::StartFrequency(float, uint8)
{
}


void Beeper::Driver::Stop()
{
    PainterMelody::self->DisableMelody();
}
