// 2024/03/20 20:05:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Source.h"


struct Record
{
    uint         crc;            // ���� ����� ����, �� ������ �����
    uint         number;
    RTCDateTime  time;
    uint8        source;         // � ������� ���� - 1/0 - �������/���������
    uint8        control_bits;   // ���� ���������� ����, ����� ���������, ��� ������ ������ �������. �� ��������� � ���������� CRC
    uint         CalculateCRC() const;
    const uint8 *Begin() const;
    const uint8 *End() const;
    bool         IsEmpty() const;
    bool         IsValidData() const;
};


// ����� ����� ��������� �������� � ���������� ������
namespace Storage
{
    void Init();

    void Save();

    // ���� receive == true, �� ����� "������"
    void Append(const RTCDateTime &, Source::E, bool receive);

    int GetCountRecords();

    Record *Get(int);
}
