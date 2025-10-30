// 2024/03/02 19:47:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Source.h"


struct Volume
{
    enum E
    {
        Low,
        Middle,
        Hi,
        Count
    };
};


struct Melody
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

    static pchar Name(E);
};


struct SettingsSource
{
    Melody::E         melody;           // Текущая мелодия
    Volume::E         volume;           // Громкость текущей мелодии
    Color::E          color;
    ModeIndication::E mode_indication;  // Режим индикации
};


// Настройки будильника
struct SettingsAlarm
{
    Melody::E         melody;
    Volume::E         volume;
    Color::E          color;
    ModeIndication::E mode_indication;
    RTCDateTime       time;
    uint8             enabled;
};


struct Settings
{
    SettingsSource sources[Source::Count];
    SettingsAlarm alarm;
};


extern Settings gset;
