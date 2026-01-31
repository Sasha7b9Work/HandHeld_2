// 2024/03/02 19:47:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


static const Settings def_set =
{
    0,      // crc32
    { {Melody::_9,  Volume::Hi, Color::Red,     ModeIndication::All},
      {Melody::_10, Volume::Hi, Color::Cyan,    ModeIndication::All},
      {Melody::_3,  Volume::Hi, Color::Yellow,  ModeIndication::All},
      {Melody::_4,  Volume::Hi, Color::Green,   ModeIndication::All},
      {Melody::_5,  Volume::Hi, Color::Blue,    ModeIndication::All},
      {Melody::_6,  Volume::Hi, Color::Magenta, ModeIndication::All}
    },
    {
        Melody::_1,
        Volume::Hi,
        Color::White,
        ModeIndication::All,
        { 0, 0, 0, 0, 0, 0 },
        1
    },
    { TimeIndication::_60s },
    0       // empty
};


Settings gset = def_set;


pchar Melody::Name(E v)
{
    static const pchar names[Count] =
    {
        "–»Õ√“ŒÕ 1",
        "–»Õ√“ŒÕ 2",
        "–»Õ√“ŒÕ 3",
        "œ≈—≈Õ ¿",
        "≈ÀŒ◊ ¿",
        " ”«Õ≈◊» ",
        "–Œƒ»Õ¿",
        "ƒ≈Õ‹ –Œ∆ƒ≈Õ»ﬂ",
        "—»–≈Õ¿ ¡€—“–.",
        "—»–≈Õ¿ Ã≈ƒÀ."
    };

    return names[v];
}


void Settings::Load()
{
    Settings set;

    if (HAL_ROM::LoadSettings(&set))
    {
        *this = set;
    }
}


void Settings::Save()
{
    Settings set;

    if (HAL_ROM::LoadSettings(&set))
    {
        if (set == *this)
        {
        }
        else
        {
            HAL_ROM::SaveSettings(this);
        }
    }
    else
    {
        HAL_ROM::SaveSettings(this);
    }
}


bool Settings::operator==(const Settings &rhs) const
{
    return GF::MemCmp(BeginData(), rhs.BeginData(), SizeData());
}


uint Settings::CalculateCRC32() const
{
    return Math::CalculateCRC32(BeginData(), SizeData());
}


const void *Settings::BeginData() const
{
    const uint *begin = (const uint *)this;

    return (const void *)(begin + 1);
}


int Settings::SizeData() const
{
    return sizeof(*this) - 4;
}


uint TimeIndication::TimeMS() const
{
    static const uint time[Count] =
    {
        10 * 1000,
        20 * 1000,
        30 * 1000,
        60 * 1000
    };

    return time[value];
}
