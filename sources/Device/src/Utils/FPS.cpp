// 2024/03/07 21:22:05 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/FPS.h"
#include "Hardware/Timer.h"
#include "Display/Font/Font.h"
#include "Display/Text.h"


namespace FPS
{
    static uint time_begin = 0;

    static uint time = 0;
}


void FPS::BeginFrame()
{
    time_begin = TIME_MS;
}


void FPS::EndFrame()
{
    time = TIME_MS - time_begin;
}


void FPS::DrawTimeFrame(int x, int y)
{
    Font::SetType(TypeFont::_5);

    Text<>("%d ms", time).Write(x, y, Color::WHITE);

    Font::SetType(TypeFont::_7);
}
