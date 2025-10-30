// 2024/04/13 13:35:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItemDef.h"


namespace PageWatch
{
    DEF_TIME(time, self, "ÂÐÅÌß", false);

    DEF_DATE(date, self, "ÄÀÒÀ", false);

    DEF_PAGE_2(pageWatch, PageMain::self, "×ÀÑÛ",
        &time,
        &date,
        nullptr,
        nullptr,
        nullptr
    );

    const Item *const self = &pageWatch;
}
