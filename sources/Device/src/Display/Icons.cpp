// 2024/03/23 13:37:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Icons.h"
#include "Display/Display7735.h"
#include "Display/Text.h"


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


void Icon::Draw(int x, int y, const Color &color) const
{
    int size = 17;

    Rect(size, size).Draw(x, y, color);
}


void IconSourceDoorBell::Draw(int x, int y, const Color &color) const
{
    Icon::Draw(x, y, color);

    Text<>("«‚").Write(x + 3, y + 4, color);
}


void IconSourceMobile::Draw(int x, int y, const Color &color) const
{
    Icon::Draw(x, y, color);

    Text<>("“Ï").Write(x + 3, y + 4, color);
}


void IconSourcePhone::Draw(int x, int y, const Color &color) const
{
    Icon::Draw(x, y, color);

    Text<>("“‰").Write(x + 3, y + 4, color);
}


void IconSourceIntercom::Draw(int x, int y, const Color &color) const
{
    Icon::Draw(x, y, color);

    Text<>("ƒÏ").Write(x + 3, y + 4, color);
}


void IconSourceSensor::Draw(int x, int y, const Color &color) const
{
    Icon::Draw(x, y, color);

    Text<>("¿Í").Write(x + 3, y + 4, color);
}
