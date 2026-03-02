// 2024/03/07 09:45:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItemDef.h"
#include "Storage/Storage.h"
#include "Display/Font/Font.h"
#include "Utils/StringUtils.h"
#include "Display/Display.h"


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

#ifdef MODEL7789

#endif

        int x = 0;

        const Record *rec = Storage::Get(top_record);
        const RTCDateTime time = rec->time;

        Font::SetSize(2);

        int y = 0;

        Text<>("%d", top_record + 1).Write(x + 5, y + 15);

        Text<>("%02d/%02d %02d:%02d",
            time.Day, time.Month, time.Hour, time.Minute).Write(x + 55, y + 15, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F))).WriteInCenter(x, y + 50, Display::WIDTH);

        Font::SetSize(1);

#ifdef MODEL7789
#endif
    }

    static void FuncDraw_History()
    {
        if (Storage::GetCountRecords() == 0)
        {
            Font::SetSize(2);

            Text<>("ЖУРНАЛ").WriteInCenter(0, SU::Y::Up(), Display::WIDTH, Color::WHITE);
            Text<>("ПУСТ").WriteInCenter(0, SU::Y::Down(), Display::WIDTH);

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

        Text<>("%d ВЫЗОВОВ", Storage::GetCountRecords()).Write(10, 35, Color::WHITE);

        Font::SetSize(1);
    }

    DEF_PAGE_0(pageHistory, PageMain::self, "ЖУРНАЛ",
        nullptr, // FuncDraw_Closed,
        FuncDraw_History,
        Func_ActionKey
    );

    const Item *const self = &pageHistory;
}
