// 2024/03/20 20:05:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Source.h"


struct Record
{
    uint         crc;            // Если здесь ноль, то запись стёрта
    uint         number;
    RTCDateTime  time;
    uint8        source;         // В старшем бите - 1/0 - принято/непринято
    uint8        control_bits;   // Сюда записываем ноль, чтобы убедиться, что запись прошла успешно. Не участвует в вычислении CRC
    uint         CalculateCRC() const;
    const uint8 *Begin() const;
    const uint8 *End() const;
    bool         IsEmpty() const;
    bool         IsValidData() const;
};


// Здесь будут храниться принятые и непринятые вызовы
namespace Storage
{
    void Init();

    void Save();

    // Если receive == true, то вызов "принят"
    void Append(const RTCDateTime &, Source::E, bool receive);

    int GetCountRecords();

    Record *Get(int);
}
