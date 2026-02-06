// 2024/03/07 09:52:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItems.h"
#include "Menu/MenuItemDef.h"


namespace PageDoorBell
{
    static void OnChange_ModeIndication_DoorBell()
    {
    }

    DEF_PAGE_SOURCE(pageDoorBell, Source::DoorBell, OnChange_ModeIndication_DoorBell);
}


namespace PageMobile
{
    static void OnChange_ModeIndication_Mobile()
    {
    }

    DEF_PAGE_SOURCE(pageMobile, Source::Mobile, OnChange_ModeIndication_Mobile);
}


namespace PageHomePhone
{
    static void OnChange_ModeIndication_PhoneHome()
    {
    }

    DEF_PAGE_SOURCE(pageHomePhone, Source::PhoneHome, OnChange_ModeIndication_PhoneHome);
}


namespace PageIntercom
{
    static void OnChange_ModeIndication_Intercom()
    {
    }

    DEF_PAGE_SOURCE(pageIntercom, Source::Intercom, OnChange_ModeIndication_Intercom);
}


namespace PageMicrophone
{
    static void OnChange_ModeIndication_Microphone()
    {
    }

    DEF_PAGE_SOURCE(pageSoundSensor, Source::Microphone, OnChange_ModeIndication_Microphone);
}


namespace PageMain
{
    static void FuncOnChangeTime()
    {
    }

    DEF_CHOICE_4(choiceTimeIndication, self, "¬–≈Ãﬂ ¬€«Œ¬¿", &gset.time_call.value,
        "10 ÒÂÍ",
        "20 ÒÂÍ",
        "30 ÒÂÍ",
        "60 ÒÂÍ",
        FuncOnChangeTime
    );


    static void FuncOnChangeVolume()
    {
    }


    DEF_CHOICE_3(choiceVolume, self, "√–ŒÃ Œ—“‹", &gset.volume,
        "Õ»« ¿ﬂ",
        "—–≈ƒÕﬂﬂ",
        "¬€—Œ ¿ﬂ",
        FuncOnChangeVolume
    );


    DEF_PAGE_10(pageMain, nullptr, "Ã≈Õﬁ",
        PageAlarm::self,
        PageWatch::self,
        PageDoorBell::self,
        PageMobile::self,
        PageHomePhone::self,
        PageIntercom::self,
        PageMicrophone::self,
        PageJournal::self,
        &choiceTimeIndication,
        &choiceVolume,
        nullptr,
        nullptr,
        nullptr
    );

    const Item *const self = &pageMain;
}
