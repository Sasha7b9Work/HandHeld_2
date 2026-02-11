// 2024/03/02 19:47:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/GlobalFunctions.h"
#include "Utils/Math.h"


static const Settings def_set =
{
    0,      // crc32
    { {Melody::_1,  Color::Red,     ModeIndication::All},
      {Melody::_2,  Color::Cyan,    ModeIndication::All},
      {Melody::_3,  Color::Yellow,  ModeIndication::All},
      {Melody::_4,  Color::Green,   ModeIndication::All},
      {Melody::_5,  Color::Blue,    ModeIndication::All},
      {Melody::_5,  Color::Magenta, ModeIndication::All}
    },
    {
        Melody::_1,
        Color::White,
        ModeIndication::All,
        { 0, 0, 0, 0, 0, 0 },
        1
    },
    2,                  // volume
    0,                  // inverse
    { TimeCall::_60s },
    0       // empty
};


Settings gset = def_set;


pchar Melody::Name(E v)
{
    static const pchar names[Count] =
    {
//      "–»Õ√“ŒÕ 1",            //  1 - 
//      "–»Õ√“ŒÕ 2",            //  2 - 
//      "–»Õ√“ŒÕ 3",            //  3-
        "œ≈—≈Õ ¿",              //  4
        "≈ÀŒ◊ ¿",               //  5
        " ”«Õ≈◊» ",             //  6
        "–Œƒ»Õ¿",               //  7
        "ƒ≈Õ‹ –Œ∆ƒ≈Õ»ﬂ"         //  8
//      "—»–≈Õ¿ ¡€—“–.",        //  9 -
//      "—»–≈Õ¿ Ã≈ƒÀ."          // 10 -
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


void Settings::Reset()
{
    gset = def_set;

    Save();
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


uint TimeCall::ToMS() const
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
