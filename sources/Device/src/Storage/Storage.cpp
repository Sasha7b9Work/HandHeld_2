// 2024/05/07 08:31:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace Storage
{
    static const uint MAX_RECORDS = 32;

    static Record records[MAX_RECORDS];

    static int num_records = 0;
}


void Storage::Init()
{
    HAL_ROM::ReadBuffer(HAL_ROM::AddressPage(HAL_ROM::PAGE_FOR_JOURNAL), records, MAX_RECORDS * (int)sizeof(Record));

    num_records = GetCountRecords();
}


void Storage::Save()
{
    HAL_ROM::ErasePage(HAL_ROM::PAGE_FOR_JOURNAL);

    HAL_ROM::WriteBuffer(HAL_ROM::AddressPage(HAL_ROM::PAGE_FOR_JOURNAL), records, num_records * (int)sizeof(Record));
}


void Storage::Append(const RTCDateTime &time, Source::E source, bool receive)
{
    Record record =
    {
        0,
        0,
        time,
        (uint8)source,
        0
    };

    if (GetCountRecords() == 0)
    {
        record.number = 1;
    }
    else
    {
        record.number = Storage::Get(GetCountRecords() - 1)->number + 1;
    }

    if (receive)
    {
        record.source |= 0x80;
    }

    if (num_records == MAX_RECORDS)
    {
        std::memmove(&records[0], &records[1], (MAX_RECORDS - 1) * sizeof(Record));
        num_records--;
    }

    records[num_records++] = record;
}


int Storage::GetCountRecords()
{
    int result = 0;

    for (uint i = 0; i < MAX_RECORDS; i++)
    {
        if (records[i].crc != (uint)(-1))
        {
            result++;
        }
    }

    return result;
}


Record *Storage::Get(int number)
{
    return &records[number];
}
