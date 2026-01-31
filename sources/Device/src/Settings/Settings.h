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


struct TimeIndication
{
    enum E
    {
        _10s,
        _20s,
        _30s,
        _60s,
        Count
    };

    uint8 value;

    uint TimeMS() const;
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
    uint           crc32;
    SettingsSource sources[Source::Count];
    SettingsAlarm  alarm;
    TimeIndication time_indication;
    uint           empty;                 // Используется для контроля записи. Должно быть ноль

    void Save();
    void Load();
    uint CalculateCRC32() const;
    const void *BeginData() const;  // Указывает на начало реальных данных (без учёта контрольной суммы, расположенной в начале)
    int SizeData() const;           // Размер реальных данных (без учёта контрольной суммы, расположенной в начале)

    bool operator ==(const Settings &rhs) const;
};


extern Settings gset;
