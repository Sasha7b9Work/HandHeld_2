// 2024/03/23 13:37:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Icons.h"
#include "Display/Text.h"
#include "Display/Display.h"
#include "Display/Primitives.h"


static IconSourceDoorBell iconSourceDoorBell;
static IconSourceMobile iconSourceMobile;
static IconSourcePhone iconSourcePhone;
static IconSourceIntercom iconSourceIntercom;
static IconSourceSensor iconSourceSensor;


Icon *sourceIcons[Source::Count] =
{
    &iconSourceDoorBell,
    &iconSourceMobile,
    &iconSourcePhone,
    &iconSourceIntercom,
    &iconSourceSensor
};


void Icon::Draw(int pos_on_display) const
{
#ifdef MODEL7789
    const int size = 46;
    const int dX = 15;
    int x = 11 + pos_on_display * (size + dX);
    int y = 11;
#else
    const int size = 17;
    int x = 11 + pos_on_display * 30;
    int y = 8;
#endif

    Rect(size, size).Draw(x, y);

#ifdef MODEL7789
    y += 6;
#endif

    Draw(x, y);
}


void IconSourceDoorBell::Draw(int x, int y) const
{
#ifdef MODEL7789
    x += 5;
#endif

    Text<>("«‚")._Write(x + 3, y + 4);
}


void IconSourceMobile::Draw(int x, int y) const
{
#ifdef MODEL7789
    x += 1;
#endif

    Text<>("“Ï")._Write(x + 3, y + 4);
}


void IconSourcePhone::Draw(int x, int y) const
{
#ifdef MODEL7789
    x += 2;
#endif

    Text<>("“‰").Write(x + 3, y + 4);
}


void IconSourceIntercom::Draw(int x, int y) const
{
    Text<>("ƒÏ").Write(x + 3, y + 4);
}


void IconSourceSensor::Draw(int x, int y) const
{
#ifdef MODEL7789
    x += 3;
#endif

    Text<>("¿Í").Write(x + 3, y + 4);
}
