// 2024/03/02 19:47:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Colors.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Source.h"


struct Melody
{
    enum E
    {
        _1,
        _2,
        _3,
        _4,
        _5,
        Count
//        _6,
//        _7,
//        _8,
//        _9,
//        _10,
//        Count
    };

    static pchar Name(E);
};


// Столько времени длится вызов
struct TimeCall
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

    uint ToMS() const;
};


struct SettingsSource
{
    Melody::E         melody;           // Текущая мелодия
    Color::E          color;
    ModeIndication::E mode_indication;  // Режим индикации
};


// Настройки будильника
struct SettingsAlarm
{
    Melody::E         melody;
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
    uint8          volume;          // 0 - низкая, 1 - средняя, 2 - высокая
    uint8          inverse;
    TimeCall       time_call;
    uint           empty;           // Используется для контроля записи. Должно быть ноль

    void Save();
    void Load();
    void Reset();
    uint CalculateCRC32() const;
    const void *BeginData() const;  // Указывает на начало реальных данных (без учёта контрольной суммы, расположенной в начале)
    int SizeData() const;           // Размер реальных данных (без учёта контрольной суммы, расположенной в начале)

    bool operator ==(const Settings &rhs) const;
};


extern Settings gset;
