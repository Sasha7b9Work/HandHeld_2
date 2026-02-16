// 2022/04/29 10:12:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#define WIN32_LEAN_AND_MEAN
#include "defines.h"
#include "GUI/Application.h"
#include "GUI/Frame.h"
#include "Device.h"
#include "Hardware/Timer.h"
#include "Modules/CMT2210AW/CMT2210AW.h"
#include "GUI/Controls/PainterLED.h"
#include "GUI/Controls/PainterMelody.h"
#include "GUI/Controls/PainterVibrator.h"


void Application::Init()
{
    Device::Init();
}


void Application::Update()
{
    Device::Update();

    CMT2210AW::CallbackOnBit();

    PainterVibrator::self->Update();
}
