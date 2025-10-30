// 2024/03/07 09:52:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItems.h"
#include "Menu/MenuItemDef.h"


#ifdef TYPE_1602
    static void OnChange_ModeIndication(Source::E source)
    {
        if (gset.sources[source].mode_indication == ModeIndication::Vibro)
        {
            gset.sources[source].mode_indication = ModeIndication::Sound_LED;
        }
        else if (gset.sources[source].mode_indication == ModeIndication::Sound_Vibro)
        {
            gset.sources[source].mode_indication = ModeIndication::All;
        }
    }
#endif


namespace PageDoorBell
{
    static void OnChange_ModeIndication_DoorBell()
    {
#ifdef TYPE_1602
        OnChange_ModeIndication(Source::DoorBell);
#endif
    }

    DEF_PAGE_SOURCE(pageDoorBell, Source::DoorBell, OnChange_ModeIndication_DoorBell);
}


namespace PageMobile
{
    static void OnChange_ModeIndication_Mobile()
    {
#ifdef TYPE_1602
        OnChange_ModeIndication(Source::Mobile);
#endif
    }

    DEF_PAGE_SOURCE(pageMobile, Source::Mobile, OnChange_ModeIndication_Mobile);
}


namespace PageHomePhone
{
    static void OnChange_ModeIndication_PhoneHome()
    {
#ifdef TYPE_1602
        OnChange_ModeIndication(Source::PhoneHome);
#endif
    }

    DEF_PAGE_SOURCE(pageHomePhone, Source::PhoneHome, OnChange_ModeIndication_PhoneHome);
}


namespace PageIntercom
{
    static void OnChange_ModeIndication_Intercom()
    {
#ifdef TYPE_1602
        OnChange_ModeIndication(Source::Intercom);
#endif
    }

    DEF_PAGE_SOURCE(pageIntercom, Source::Intercom, OnChange_ModeIndication_Intercom);
}


namespace PageMicrophone
{
    static void OnChange_ModeIndication_Microphone()
    {
#ifdef TYPE_1602
        OnChange_ModeIndication(Source::Microphone);
#endif
    }

    DEF_PAGE_SOURCE(pageSoundSensor, Source::Microphone, OnChange_ModeIndication_Microphone);
}


DEF_PAGE_8(pageMain, nullptr, "лемч",
    PageAlarm::self,
    PageWatch::self,
    PageDoorBell::self,
    PageMobile::self,
    PageHomePhone::self,
    PageIntercom::self,
    PageMicrophone::self,
    PageJournal::self,
    nullptr,
    nullptr,
    nullptr
);


const Item * const PageMain::self = &pageMain;
