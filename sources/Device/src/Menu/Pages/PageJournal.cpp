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
    static int num_top_record = 0;          // Номер записи, которая размещена наверху экрана

    void FuncDraw_Closed();

    static void DrawRecord(int y, const Record *rec)
    {
        const RTCDateTime time = rec->time;

        int x = 0;

        Text<>("%d", num_top_record + 1).Write(x + 5, y + 15);

        Text<>("%02d/%02d %02d:%02d",
            time.Day, time.Month, time.Hour, time.Minute).Write(x + 55, y + 15, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F))).WriteInCenter(x, y + 50, Display::WIDTH);

    }

    static void DrawRecords()
    {
        if (num_top_record >= Storage::GetCountRecords())
        {
            return;
        }

#ifdef MODEL7789
        Font::StoreType();
        Font::SetSmallType();
#endif

        Font::SetSize(2);

        DrawRecord(0, Storage::Get(num_top_record));

        Font::SetSize(1);

#ifdef MODEL7789
        Font::RestoreType();
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
            DrawRecords();
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
            num_top_record--;
        }
        else if (action.key == Key::Down)
        {
            num_top_record++;
        }

        if (num_top_record < 0)
        {
            num_top_record = Storage::GetCountRecords() - 1;
        }
        else if (num_top_record >= Storage::GetCountRecords())
        {
            num_top_record = 0;
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
