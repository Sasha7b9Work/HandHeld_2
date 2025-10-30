// 2024/03/23 17:28:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Source.h"
#include "Display/Icons.h"
#include "Hardware/Timer.h"
#include "Hardware/Vibrato.h"
#include "Storage/Storage.h"
#include "Modules/LED/LED.h"
#include "Modules/Beeper/Beeper.h"
#include "Modules/CMT2210AW/CMT2210AW.h"
#include "Keyboard/Keyboard.h"
#include <cstring>


//const led_vibro_const_t LED_VIBRO_TIMINGS[] = {
//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 200 / 5, .pulse_period = 320 / 5, .pulses_in_packet = 2, .pause_between_packet = 4000 / 5, .total_duration = 38000 / 5},  //передатчик дверного звонка
//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 240 / 5, .pulse_period = 380 / 5, .pulses_in_packet = 4, .pause_between_packet = 5500 / 5, .total_duration = 31000 / 5},  //передатчик телефона (старое знач.5500)
//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 240 / 5, .pulse_period = 450 / 5, .pulses_in_packet = 3, .pause_between_packet = 3500 / 5, .total_duration = 31000 / 5},  //передатчик домофона (старое знач.6500)
//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 180 / 5, .pulse_period = 600 / 5, .pulses_in_packet = 5, .pause_between_packet = 1500 / 5, .total_duration = 31000 / 5},  //передатчик плача ребенка

//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 40 / 5, .pulse_period = 110 / 5, .pulses_in_packet = 255, .pause_between_packet = 55 / 5,   .total_duration = 60000 / 5},  //передатчик пожарной сигнализации
//{.led_pulse_duration = 100 / 5, .vibro_pulse_duration = 40 / 5, .pulse_period = 400 / 5, .pulses_in_packet = 6, .pause_between_packet = 600 / 5,  .total_duration = 30000 / 5},  //передатчик пейджера
//{.led_pulse_duration = 15 / 5, .vibro_pulse_duration = 160 / 5, .pulse_period = 1000 / 5, .pulses_in_packet = 1, .pause_between_packet = 0,      .total_duration = 1000 / 5},  //вспомог.индикация при включении
//{.led_pulse_duration = 150 / 5, .vibro_pulse_duration = 300 / 5, .pulse_period = 100 / 5, .pulses_in_packet = 2, .pause_between_packet = 0,      .total_duration = 600 / 5},  //вспомог.индикация при выключении



const SourceScript SourceScript::scripts[Source::Count] =
{
    { 100, 200, 320, 2, 4000 },
    { 100, 100, 400, 3, 4000 },
    { 100, 240, 380, 4, 5500 },
    { 100, 240, 450, 5, 3500 },
    { 100, 180, 600, 6, 1500 },
    { 100, 180, 600, 6, 1500 }
};


bool SourceScript::GetForLED(Source::E source, uint _time)
{
    const SourceScript &script = scripts[source];

    int time = (int)(_time % script.PeriodPacket());            // Находим наш пакет

    for (int i = 0; i < script.num_pulses; i++)
    {
        if (time <= script.led_duration + 100 && time >= 100)
        {
            return true;
        }

        time -= script.period;
    }

    return false;
}


bool SourceScript::GetForVibro(Source::E source, uint _time)
{
    const SourceScript &script = scripts[source];

    int time = (int)(_time % script.PeriodPacket());            // Находим наш пакет

    for (int i = 0; i < script.num_pulses; i++)
    {
        if (time < script.vibro_duration + 100 && time >= 100)
        {
            return true;
        }

        time -= script.period;
    }

    return false;
}


uint SourceScript::PeriodPacket() const
{
    return (uint)period * (uint)num_pulses + (uint)pause_packet;
}


Source::E Source::Queue::buffer[Source::Count] = { Source::Count, Source::Count, Source::Count, Source::Count, Source::Count, Source::Count };
int Source::Queue::size = 0;

Source::Queue::Time Source::Queue::time_recv[Source::Count];

#ifdef TYPE_1602
void Source::DrawIcon(int , int , const Color &) const
{
}
#else
void Source::DrawIcon(int x, int y, const Color &color) const
{
    sourceIcons[value]->Draw(x, y, color);
}
#endif


static bool need_received[Source::Count] = { false, false, false, false, false, false };


pchar Source::Name(E v)
{
    static const pchar names[Count] =
    {
        "ДВЕРНОЙ ЗВОНОК",
        "МОБИЛЬНЫЙ ТЕЛЕФОН",
        "ДОМАШНИЙ ТЕЛЕФОН",
        "ДОМОФОН",
        "МИКРОФОН",
        "TECT"
    };

    return names[v];
}


pchar Source::NameSmall(E v)
{
    static const pchar names[Count] =
    {
        "ДВЕРНОЙ ЗВОНОК",
        "МОБИЛ. ТЕЛЕФОН",
        "ДОМАШ. ТЕЛЕФОН",
        "ДОМОФОН",
        "МИКРОФОН",
        "TECT"
    };

    return names[v];
}


void Source::Receive(E type)
{
    need_received[type] = true;
}


bool Source::ExistReceived()
{
    for (int i = 0; i < Count; i++)
    {
        if (need_received[i])
        {
            return true;
        }
    }

    return GetCountReceived() != 0;
}


bool Source::IsReceived(E type)
{
    return Queue::IsConsist(type);
}


void Source::Update()
{
    if (CMT2210AW::IsEnabled())
    {
        return;
    }

    for (int i = 0; i < Source::Count; i++)
    {
        if (need_received[i])
        {
            need_received[i] = false;

            // Время предыдущего приёма сигнала
            static uint time_prev_signal[Source::Count] = { 0, 0, 0, 0, 0, 0 };

            // Время, в течение которого не нужно повторно принимать событие
            static const uint time_pause[Source::Count] =
            {
                1000,
                10000,
                10000,
                10000,
                15000,
                10000
            };

            if (time_prev_signal[i] == 0 ||
                (TIME_MS > time_prev_signal[i] + time_pause[i]))
            {
                time_prev_signal[i] = TIME_MS;

                Queue::Push((Source::E)i);
            }
        }
    }

    bool enabled = false;

    for (int i = 0; i < Source::Count; i++)
    {
        if (IsReceived((Source::E)i))
        {
            enabled = true;
        }
    }

    if (!enabled && !PCF8563::IsAlarmed())
    {
        Vibrato::Disable();
        LED::Disable();
        Beeper::Stop();
    }
}


void Source::Queue::Push(Source::E type)
{
    if (IsConsist(type))
    {
        for (int i = 0; i < size; i++)
        {
            if (buffer[i] == type)
            {
                time_recv[type].Set(TIME_MS, PCF8563::GetDateTime());
                break;
            }
        }
    }
    else
    {
        buffer[size++] = type;
        time_recv[type].Set(TIME_MS, PCF8563::GetDateTime());
    }

    const SettingsSource &source = gset.sources[type];

    if (ModeIndication::ConsistSound(source.mode_indication))
    {
        Beeper::_Play((TypeSound::E)source.melody, (uint8)source.volume);
    }

    if (ModeIndication::ConsistVibro(source.mode_indication))
    {
        Vibrato::Enable();
    }

    if (ModeIndication::ConsistLED(source.mode_indication))
    {
        LED::Enable();
    }

}


Source::E Source::Queue::At(int i)
{
    return buffer[i];
}


bool Source::Queue::IsConsist(Source::E source)
{
    DeleteOld();

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == source)
        {
            return true;
        }
    }

    return false;
}


void Source::Queue::DeleteOld()
{
    if (size)
    {
        for (int i = 0; i < size; i++)
        {
            if (TIME_MS > time_recv[buffer[i]].GetMS() + TimeDestroy(Current()))
            {
                Remove(i, false);

                //Установить таймаут свечения дисплея после окончания вызова
                Keyboard::CallbackFromInterrupt();
            }
        }
    }
}


void Source::CancelFirst()
{
    Queue::Remove(0, true);
}


void Source::Queue::Remove(int index, bool received)
{
    if (index < Size())
    {
        Source::E source = buffer[index];

        Storage::Append(time_recv[source].GetRTC(), source, received);

        size--;

        if (size)
        {
            std::memmove(buffer + index, buffer + index + 1, (uint)size * sizeof(Source::E));
        }
    }
}


int Source::GetCountReceived()
{
    return Queue::Size();
}


Source::E Source::Current()
{
    return Queue::At(0);
}


uint Source::TimeDestroy(E v)
{
    static const uint times[Count] =
    {
        38000,
        31000,
        31000,
        31000,
        31000,
        10000
    };

    return times[v];
}


bool ModeIndication::ConsistSound(E mode)
{
    return mode == All || mode == Sound || mode == Sound_LED || mode == Sound_Vibro;
}


bool ModeIndication::ConsistLED(E mode)
{
    return mode == All || mode == LED || mode == Sound_LED || mode == LED_Vibro;
}


bool ModeIndication::ConsistVibro(E mode)
{
#ifdef TYPE_1602
    (void)mode;

    return true;
#else
    return mode == All || mode == Vibro || mode == Sound_Vibro || mode == LED_Vibro;
#endif
}
