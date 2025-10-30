// 2024/04/13 13:27:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItemDef.h"
#include "Modules/PCF8563/PCF8563.h"


namespace PageAlarm
{
    static void FuncOnChange()
    {
        PCF8563::SetAlarm(&gset.alarm.time, gset.alarm.enabled != 0);
    }

    DEF_CHOICE_2(choiceEnabled, self, "¬ Àﬁ◊≈ÕŒ", &gset.alarm.enabled,
        "Õ≈“",
        "ƒ¿",
        FuncOnChange
    );

    DEF_TIME(time, self, "¬–≈Ãﬂ", true);

    static void OnChange_ModeIndication()
    {
#ifdef TYPE_1602
        if (gset.alarm.mode_indication == ModeIndication::Vibro)
        {
            gset.alarm.mode_indication = ModeIndication::Sound_LED;
        }
        else if (gset.alarm.mode_indication == ModeIndication::Sound_Vibro)
        {
            gset.alarm.mode_indication = ModeIndication::All;
        }
#endif
    }
    DEF_CHOICE_MODE_INDICATION(gset.alarm.mode_indication, OnChange_ModeIndication);

    DEF_CHOICE_MELODY(gset.alarm.melody);

    DEF_CHOICE_VOLUME(gset.alarm.volume);

    DEF_CHOICE_COLOR(gset.alarm.color);

    DEF_PAGE_6(pageAlarm, PageMain::self, "¡”ƒ»À‹Õ» ",
        &choiceEnabled,
        &time,
        &choiceModeIndication,
        &choiceMelody,
        &choiceVolume,
        &choiceColor,
        nullptr,
        nullptr,
        nullptr
    );

    const Item *const self = &pageAlarm;
}
