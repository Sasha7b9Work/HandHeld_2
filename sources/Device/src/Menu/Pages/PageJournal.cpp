// 2024/03/07 09:45:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItemDef.h"
#include "Storage/Storage.h"
#include "Display/Font/Font.h"
#include "Hardware/HAL/HAL.h"
#ifdef TYPE_1602
    #include "Display/Display1602.h"
#else
    #include "Display/Display7735.h"
#endif


namespace PageJournal
{
    static int top_record = 0;

    void FuncDraw_Closed();

    static void DrawRecord()
    {
        if (top_record >= Storage::GetCountRecords())
        {
            return;
        }

        int x = 0;

        const Record *rec = Storage::Get(top_record);
        const RTCDateTime time = rec->time;

        Font::SetSize(2);

#ifdef TYPE_1602
        Text<>("%02d  %02d/%02d %02d:%02d",
            top_record + 1, time.Day, time.Month, time.Hour, time.Minute).Write(0, 0, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F))).WriteInCenter(x, 1, Display::WIDTH);
#else
        int y = 0;

        Text<>("%d", top_record + 1).Write(x + 5, y + 15);

        Text<>("%02d/%02d %02d:%02d",
            time.Day, time.Month, time.Hour, time.Minute).Write(x + 55, y + 15, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F))).WriteInCenter(x, y + 50, Display::WIDTH);
#endif

        Font::SetSize(1);
    }

    static void FuncDraw_History()
    {
        if (Storage::GetCountRecords() == 0)
        {
            Font::SetSize(2);

            Text<>("∆”–Õ¿À").WriteInCenter(0, HAL::Is1602() ? 0 : 20, Display::WIDTH, Color::WHITE);
            Text<>("œ”—“").WriteInCenter(0, HAL::Is1602() ? 1 : 50, Display::WIDTH);

            Font::SetSize(1);
        }
        else
        {
            DrawRecord();
        }
    }

    static bool Func_ActionKey(const Action &action)
    {
        if (action.key == Key::Menu || action.key == Key::Cancel)
        {
            return false;
        }
        else if (action.key == Key::Up)
        {
            top_record--;
        }
        else if (action.key == Key::Down)
        {
            top_record++;
        }

        if (top_record < 0)
        {
            top_record = Storage::GetCountRecords() - 1;
        }
        else if (top_record >= Storage::GetCountRecords())
        {
            top_record = 0;
        }

        return true;
    }

    void FuncDraw_Closed()
    {
        Font::SetSize(2);

        Text<>("%d ¬€«Œ¬Œ¬", Storage::GetCountRecords()).Write(10, 35, Color::WHITE);

        Font::SetSize(1);
    }

    DEF_PAGE_0(pageHistory, PageMain::self, "∆”–Õ¿À",
        nullptr, // FuncDraw_Closed,
        FuncDraw_History,
        Func_ActionKey
    );

    const Item *const self = &pageHistory;
}
