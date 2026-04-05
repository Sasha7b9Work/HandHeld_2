// 2024/03/07 09:52:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItems.h"
#include "Menu/MenuItemDef.h"
#include "Display/Display.h"
#include "Hardware/Power.h"


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


    static void FuncOnChangeInverse()
    {
        Color::Init();

        Display::Refresh();
    }


    DEF_CHOICE_2(choiceInverse, self, "»Õ¬≈–—»ﬂ", &gset.inverse,
        "Œ“ À",
        "¬ À",
        FuncOnChangeInverse
    );


    static uint8 index_time_voltage = 0;

    static void FuncOnChangeTimeVoltage()
    {
        static const uint num[5] =
        {
            1,
            2,
            5,
            10,
            60
        };

        Power::time_control_ms = num[index_time_voltage] * 1000;
    }


    DEF_CHOICE_5(choiceTimeVoltage, self, "T Ì‡ÔˇÊÂÌËˇ", &index_time_voltage,
        "1",
        "2",
        "5",
        "10",
        "60",
        FuncOnChangeTimeVoltage
    );

#ifdef NEED_TO_CHANGE_VOLTAGE_MEASUREMENTS

    DEF_PAGE_12(pageMain, nullptr, "Ã≈Õﬁ",
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
        &choiceInverse,
        &choiceTimeVoltage,
        nullptr,
        nullptr,
        nullptr
    );

#else

    DEF_PAGE_11(pageMain, nullptr, "Ã≈Õﬁ",
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
        &choiceInverse,
        nullptr,
        nullptr,
        nullptr
    );

#endif

    const Item *const self = &pageMain;
}
