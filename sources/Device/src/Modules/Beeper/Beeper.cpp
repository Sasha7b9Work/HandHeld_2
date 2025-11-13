// 2024/03/18 15:45:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/Beeper/driverBeeper.h"
#include "Modules/Beeper/Beeper.h"
#include "Hardware/Timer.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <cmath>
#ifdef GUI
    #include "GUI/Controls/PainterMelody.h"
#endif
#include <gd32e23x.h>


struct Note
{
    uint16 frequency;       // В герцах
    uint8  duration;        // В 100 мс
    uint8  velocity;        // 10 градаций - от 0 до 9
};


static const float freqs[] =
{
    0.0f,  262.63f,  277.18f,  293.66f,  311.13f,  329.63f,  349.23f,  369.99f,  392.00f,  415.30f,  440.0f,  466.16f,  493.88f,
           523.25f,  554.36f,  587.32f,  622.26f,  659.26f,  698.46f,  739.98f,  784.00f,  830.60f,  880.0f,  932.32f,  987.75f,
          1046.50f, 1108.70f, 1174.60f, 1244.50f, 1318.50f, 1396.90f, 1480.00f, 1568.00f, 1661.20f, 1720.00f, 1864.60f, 1975.50f
};


static const uint pauses[] =
{
    32, 64, 128, 256, 512, 1024, 2048, 4096
};


struct Sound
{
    const Note *const  notes;       // В конце мелодии нулевые значения
    const uint8 *const composite;   // Здесь объединённые индексы частот и длительностей

    static void Start();

    static void Update();

    static const Sound *current;
    static const Sound *sounds[TypeSound::Count];

private:

    static int num_note;
    static uint time_note_start;    // В миллисекундах

    // Полное время звучания проигрываемой ноты
    static uint TimeNoteFull();

    // Столько нот в звуке
    static int NumberNotes();

    // Возвращает частоту текущей ноты
    static float GetFrequency();
};


#include "Modules/Beeper/Sounds/sound1.h"
#include "Modules/Beeper/Sounds/sound2.h"
#include "Modules/Beeper/Sounds/sound3.h"
#include "Modules/Beeper/Sounds/sound4.h"
#include "Modules/Beeper/Sounds/sound5.h"
#include "Modules/Beeper/Sounds/sound6.h"
#include "Modules/Beeper/Sounds/sound7.h"
#include "Modules/Beeper/Sounds/sound8.h"
#include "Modules/Beeper/Sounds/sound9.h"
#include "Modules/Beeper/Sounds/sound10.h"


const Sound *Sound::sounds[TypeSound::Count] =
{
    &sound1, &sound2, &sound3, &sound4, &sound5, &sound6, &sound7, &sound8, &sound9, &sound10
};


const Sound *Sound::current = nullptr;
int Sound::num_note = 0;
uint Sound::time_note_start = 0;


namespace Beeper
{
    static bool is_running = false;
    static bool need_running = false;
    static uint time_start = 0;       // В это время нужно запустить
    static uint8 volume = 0;

    static PinOut pinMUTE(GPIOB, GPIO_PIN_1);
}


void Beeper::Init()
{
    Beeper::Driver::Init();

    pinMUTE.Init();

    pinMUTE.ToHi();
}


void Beeper::Play(TypeSound::E type, uint8 _volume)
{
    if (is_running)
    {
        return;
    }

#ifdef GUI

    PainterMelody::self->EnableMelody(type);

#endif

    is_running = true;

    need_running = true;

    time_start = TIME_MS + 100;

    Sound::current = Sound::sounds[type];

    volume = _volume;

    pinMUTE.ToLow();
}


void Sound::Start()
{
    num_note = 0;

    time_note_start = TIME_MS;

    Beeper::is_running = true;

    Beeper::Driver::StartFrequency(GetFrequency(), Beeper::volume);
}


void Beeper::Update()
{
#ifdef GUI

    PainterMelody::self->Update();

#else

    if (need_running)
    {
        if (TIME_MS >= time_start)
        {
            Sound::Start();

            need_running = false;
        }
    }

#endif
}


void Beeper::Stop()
{
    is_running = false;

    Beeper::Driver::Stop();

    pinMUTE.ToHi();
}


bool Beeper::IsRunning()
{
    return is_running;
}


float Sound::GetFrequency()
{
    if (current->notes)
    {
        return (float)current->notes[num_note].frequency;
    }

    uint8 code = (uint8)(current->composite[num_note] & 0x1F);

    if (current->composite[num_note] != 0 && code == 0)
    {
        num_note++;

        code = (uint8)(current->composite[num_note] & 0x1F);
    }

    return freqs[code];
}


void Sound::Update()
{
    if (TIME_MS - time_note_start >= TimeNoteFull())
    {
        num_note++;

        if (num_note >= NumberNotes())
        {
            Beeper::Stop();

            if (Source::GetCountReceived() || PCF8563::IsAlarmed())
            {
                Sound::Start();
            }
        }
        else
        {
            Beeper::Driver::StartFrequency(GetFrequency(), Beeper::volume);

            time_note_start = TIME_MS;
        }
    }
}


uint Sound::TimeNoteFull()
{
    if (current->notes)
    {
        return current->notes[num_note].duration * 100U;
    }

    uint8 code = (uint8)((current->composite[num_note] >> 5) & 0x7);

    return pauses[code];
}


int Sound::NumberNotes()
{
    if (current->notes)
    {
        for (int i = 0; ; i++)
        {
            if (current->notes[i].frequency == 0)
            {
                return i;
            }
        }
    }

    for (int i = 0; ; i++)
    {
        if (current->composite[i] == 0)
        {
            return i;
        }
    }
}


void Beeper::CallbackOnTimer()
{
    Sound::Update();
}
