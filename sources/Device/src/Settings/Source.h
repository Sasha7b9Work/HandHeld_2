// 2024/03/23 17:28:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/PCF8563/PCF8563.h"


// �������� �������
struct Source
{
    enum E
    {
        DoorBell,       // ������� ������
        Mobile,         // ��������� �������
        PhoneHome,      // �������� �������
        Intercom,       // �������
        Microphone,     // ����������� ������
        Test,
        Count
    };

    Source(E v) : value(v) { }

    // ���������� ������ ��� ����������
    void DrawIcon(int x, int y, const Color &color = Color::NUMBER) const;

    // ���������� ��� ��������� �������
    static void Receive(E);

    static bool ExistReceived();

    // �������� ������ �������� �����
    static void CancelFirst();

    // ���������� true, ���� ���� ���
    static bool IsReceived(E);

    // ���������� ������ ��������
    static E Current();

    // ���� ������, ��������� ���������
    static int GetCountReceived();

    static void Update();

    static pchar Name(E);

    static pchar NameSmall(E);

    // ����� ������
    static uint TimeDestroy(E);

private:

    Source::E value;

    // ������� ������ ������, ��� ������ � �������
    struct Queue
    {
        // �������� � �����
        static void Push(Source::E);

        static Source::E At(int);

        static bool IsConsist(Source::E);

        static int Size()
        {
            return size;
        }

        static bool IsEmpty()
        {
            return Size() == 0;
        }

        static void Remove(int index, bool received);

    private:

        struct Time
        {
        private:
            uint        ms = 0;
            RTCDateTime rtc;
        public:
            void Set(uint _ms, const RTCDateTime &_rtc)
            {
                ms = _ms;
                rtc = _rtc;
            }

            uint GetMS() const
            {
                return ms;
            }

            RTCDateTime GetRTC() const
            {
                return rtc;
            }
        };

        static Source::E buffer[Source::Count];         // ����� �������� ������� � ��� �������, ��� �����������
        static Time time_recv[Source::Count];           // ����� ������ ������
        static int size;

        // ������� ��, ���� �������� ������� ����
        static void DeleteOld();
    };
};


struct SourceScript
{
    int16 led_duration;
    int16 vibro_duration;
    int16 period;           // ���������� ����� ���������
    uint8 num_pulses;       // ���������� �������
    int16 pause_packet;

    uint PeriodPacket() const;

    static const SourceScript scripts[Source::Count];

    static bool GetForLED(Source::E, uint time);
    static bool GetForVibro(Source::E, uint time);
};


struct ModeIndication
{
    enum E
    {
        Sound,
        LED,
        Vibro,
        Sound_LED,
        Sound_Vibro,
        LED_Vibro,
        All,
        Count
    };

    static bool ConsistSound(E);
    static bool ConsistLED(E);
    static bool ConsistVibro(E);
};
