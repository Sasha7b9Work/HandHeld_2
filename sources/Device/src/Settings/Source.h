// 2024/03/23 17:28:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modules/PCF8563/PCF8563.h"


// Источинк сигнала
struct Source
{
    enum E
    {
        DoorBell,       // Дверной звонок
        Mobile,         // Мобильный телефон
        PhoneHome,      // Домашний телефон
        Intercom,       // Домофон
        Microphone,     // Акстический датчик
        Test,
        Count
    };

    Source(E v) : value(v) { }

    // Нарисовать иконку для оповещения
    void DrawIcon(int x, int y, const Color &color = Color::NUMBER) const;

    // Вызывается при получении сигнала
    static void Receive(E);

    static bool ExistReceived();

    // Отменить первый принятый вызов
    static void CancelFirst();

    // Возвращает true, если приём идёт
    static bool IsReceived(E);

    // Возвращает первый принятый
    static E Current();

    // Есть звонки, требующие отработки
    static int GetCountReceived();

    static void Update();

    static pchar Name(E);

    static pchar NameSmall(E);

    // Время отмены
    static uint TimeDestroy(E);

private:

    Source::E value;

    // Который первый входит, тот первый и выходит
    struct Queue
    {
        // Добавить в хвост
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

        static Source::E buffer[Source::Count];         // Здесь принятые события в том порядке, как принимались
        static Time time_recv[Source::Count];           // Время начала аларма
        static int size;

        // Удалить те, срок действия которых истёк
        static void DeleteOld();
    };
};


struct SourceScript
{
    int16 led_duration;
    int16 vibro_duration;
    int16 period;           // Расстояние между вспышками
    uint8 num_pulses;       // Количество вспышек
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
