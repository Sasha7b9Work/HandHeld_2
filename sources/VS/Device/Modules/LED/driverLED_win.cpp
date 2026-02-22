// 2024/03/20 19:26:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "GUI/Controls/PainterLED.h"
#include "GUI/Frame.h"
#include "Modules/LED/LED.h"
#include "Modules/LED/driverLED.h"


void LED::Driver::Init()
{
    PainterLED::Create(Frame::self, Painter::size_indicator);

#ifdef MODEL7735
    const int x = 550;
#endif
#ifdef MODEL7789
    const int x = 900;
#endif

    PainterLED::self->SetPosition({ x, 68 });
}


void LED::Driver::On(Color::E)
{
    PainterLED::self->SetEnabled();
}


void LED::Driver::Off()
{
    PainterLED::self->SetDisabled();
}


bool LED::Driver::IsFired()
{
    return PainterLED::self->IsEnabled();
}
