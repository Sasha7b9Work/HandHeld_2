// 2024/03/02 19:47:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/Vibrato.h"
#include "Display/Icons.h"
#include <cstdlib>


Settings gset =
{
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
    }
};


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
