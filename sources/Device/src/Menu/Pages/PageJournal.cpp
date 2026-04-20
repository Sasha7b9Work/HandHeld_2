// 2024/03/07 09:45:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Pages/Pages.h"
#include "Menu/MenuItemDef.h"
#include "Storage/Storage.h"
#include "Display/Font/Font.h"
#include "Utils/StringUtils.h"
#include "Display/Display.h"
#include "Display/Primitives.h"


namespace PageJournal
{
    static int num_top_record = 0;          // Номер записи, которая размещена наверху экрана

    void FuncDraw_Closed();

    static void DrawRecord(int y, const Record *rec)
    {
        const RTCDateTime time = rec->time;

#ifdef MODEL7735

        int x = 0;

        Text<>("%d", rec->number + 1).Write(x + 5, y + 15, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        Text<>("%02d/%02d %02d:%02d",
            time.Day, time.Month, time.Hour, time.Minute).Write(x + 55, y + 15);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F))).WriteInCenter(x, y + 50, Display::WIDTH);
#else
        Font::StoreType();
        Font::SetSmallType();

        int x = 20;

        Text<>("%d", rec->number + 1)._Write(x + 5, y + 15, (rec->source & 0x80) ? Color::GREEN : Color::RED);

        x += 40;

        Text<>("%02d/%02d %02d:%02d",
            time.Day, time.Month, time.Hour, time.Minute)._Write(x, y + 15);

        Text<>(Source::NameSmall((Source::E)(rec->source & 0x7F)))._Write(x + 20, y + 37);

        Font::RestoreType();
#endif
    }

    static void DrawRecords()
    {
        if (num_top_record >= Storage::GetCountRecords())
        {
            return;
        }

        Font::SetSize(2);

#ifdef MODEL7735
        DrawRecord(0, Storage::Get(num_top_record));
#else
        int drawing_records = 0;

        int y = 0;

        for (int i = num_top_record; i < Storage::GetCountRecords() && drawing_records < 4; i++, drawing_records++)
        {
            DrawRecord(y * 55, Storage::Get(i));

            y++;
        }

        {
            // Рисуем индикактор заполнения справа

            const int width = 10;
            const int dy = 10;
            const int height = Display::HEIGHT - dy;

            int x0 = Display::WIDTH - width - 10;
            int y0 = dy / 2;

            Rect(width, height).Draw(x0, y0, Color::GRAY);

            {
                int h = (int)((float)drawing_records * height / (float)Storage::GetCountRecords() + 0.5f);

                float part = (float)num_top_record / (float)Storage::GetCountRecords();

                Rect(width + 6, h).Fill(x0 - 3, (int)((float)y0 + part * height));
            }
        }
#endif

        Font::SetSize(1);
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

        Text<>("%d ВЫЗОВОВ", Storage::GetCountRecords())._Write(10, SU::Y::Center(), Color::WHITE);

        Font::SetSize(1);
    }

    DEF_PAGE_0(pageHistory, PageMain::self, "ЖУРНАЛ",
        nullptr, // FuncDraw_Closed,
        FuncDraw_History,
        Func_ActionKey
    );

    const Item *const self = &pageHistory;
}
