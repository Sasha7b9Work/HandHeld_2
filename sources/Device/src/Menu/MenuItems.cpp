// 2024/03/02 13:56:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/MenuItems.h"
#include "Menu/Menu.h"
#include "Display/Font/Font.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Utils/StringUtils.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#ifdef TYPE_1602
    #include "Display/Display1602.h"
#else
    #include "Display/Display7735.h"
#endif


void Item::Draw() const
{
    if (IsPage())
    {
        GetPage()->Draw();
    }
    else if (IsDateTime())
    {
        GetDateTime()->Draw();
    }
    else if (IsChoice())
    {
        GetChoice()->Draw();
    }
}


void Page::Draw() const
{
    if (data->item->IsOpened())
    {
        if (data->func_draw_opened)
        {
            data->func_draw_opened();
        }
        else
        {
            const Item *current_item = CurrentItem();

            if (current_item)
            {
                current_item->Draw();
            }
        }
    }
    else
    {
        Font::SetSize(2);

        Text<> title = data->item->Title();

        int num_words = SU::NumWordsInString(title.c_str());

        if (num_words == 1)
        {
            title.WriteInCenter(0, HAL::Is1602() ? 0 : 30, Display::WIDTH, Color::GREEN);
        }
        else if(num_words == 2)
        {
            char buffer[32];

            Text<>(SU::GetWordFromString(title.c_str(), 1, buffer)).WriteInCenter(0, HAL::Is1602() ? 0 : 15, Display::WIDTH, Color::GREEN);

            Text<>(SU::GetWordFromString(title.c_str(), 2, buffer)).WriteInCenter(0, HAL::Is1602() ? 1 : 45, Display::WIDTH);
        }

        Font::SetSize(1);

        if (data->func_draw_closed)
        {
            data->func_draw_closed();
        }
    }
}


void Choice::Draw() const
{
    if (data->item->IsOpened())
    {

    }
    else
    {
        Font::SetSize(2);

        data->item->Title().WriteInCenter(0, HAL::Is1602() ? 0 : 15, Display::WIDTH, Color::GREEN);

        int index = (int)(*data->value);

        pchar text = data->names[index];

        Color color = Color::WHITE;

        if (data->colors)
        {
            color = Color(data->colors[index]);
        }

        Text<>(text).WriteInCenter(0, HAL::Is1602() ? 1 : 45, Display::WIDTH, color);

        Font::SetSize(1);
    }
}


void DateTime::DrawField(int x, int y, Text<> &text, bool selected) const
{
    bool need_draw = true;

#ifdef TYPE_1602

    if (selected)
    {
        if (((TIME_MS / 250) % 3) == 2)     // Âûךכ‏קאול םא 250 לס קונוח ךאזהûו 750
        {
            need_draw = false;
        }
    }

#else
    const int width = 42;
    const int height = 35;

    Rect(width, height).Draw(x, y, selected ? Color::BLACK : Color::WHITE);
    Rect(width - 2, height - 2).Fill(x + 1, y + 1, selected ? Color::WHITE : Color::BLACK);
    if (selected)
    {
        Rect(width + 2, height + 2).Draw(x - 1, y - 1, Color::WHITE);
    }

#endif

    if (need_draw)
    {
        Font::SetSize(4);
        text.Write(x + 3, HAL::Is1602() ? 1 : (y + 3), selected ? Color::BLACK : Color::WHITE);
        Font::SetSize(1);
    }
}


void DateTime::Draw() const
{
    if (data->item->IsOpened())
    {
        Font::SetSize(2);

        Text<>("ÓÑÒÀÍÎÂÊÀ").WriteInCenter(0, HAL::Is1602() ? 0 : 10, Display::WIDTH, Color::WHITE);

        Font::SetSize(1);

        int values[3] =
        {
            data->date_time->Day,
            data->date_time->Month,
            data->date_time->Year
        };

        if (data->is_time)
        {
            values[0] = data->date_time->Hour;
            values[1] = data->date_time->Minute;
        }

        const int y = 32;

#ifdef TYPE_1602
        int x[3] = { 1, 4, 7 };

        if (data->is_time)
        {
            x[0] = 3;
            x[1] = 6;
        }
#else
        int x[3] = { 5, 58, 111 };

        if (data->is_time)
        {
            x[0] = 30;
            x[1] = 80;
        }
#endif

        for (int i = 0; i < NumFields(); i++)
        {
            Text <>text("%02d", values[i]);

            DrawField(x[i], y, text, i == *data->field);
        }
    }
    else
    {
        Font::SetSize(2);

        data->item->Title().WriteInCenter(0, HAL::Is1602() ? 0 : 30, Display::WIDTH, Color::GREEN);

        Font::SetSize(1);
    }
}


Text<> Item::Title() const
{
    const Page *page_keeper = data->keeper->GetPage();

    for (int i = 0; ; i++)
    {
        if (page_keeper->data->items[i] == this)
        {
            return Text<>(data->title);
        }
    }
}


const DataItem *Page::GetDataItem() const
{
    return data->item->data;
}


bool Item::IsOpened() const
{
    if (data->keeper == nullptr)
    {
        return true;
    }

    return *data->opened != 0;
}


const Item *Page::CurrentItem() const
{
    return data->items[*data->current_item];
}


void Item::ApplyAction(const Action &action) const
{
    if (IsPage())
    {
        GetPage()->ApplyAction(action);
    }
    else if (IsDateTime())
    {
        GetDateTime()->ApplyAction(action);
    }
    else if (IsChoice())
    {
        GetChoice()->ApplyAction(action);
    }
}


void Page::ApplyAction(const Action &action) const
{
    bool need_apply = true;

    if (data->func_action_key)
    {
        if (data->func_action_key(action))
        {
            need_apply = false;
        }
    }

    if (need_apply)
    {
        if (action.key == Key::Up)
        {
            PrevCurrentItem();
        }
        else if (action.key == Key::Down)
        {
            NextCurrentItem();
        }
        else if (action.key == Key::Menu)
        {
            if (NumItems())
            {
                if (CurrentItem()->IsChoice())
                {
                    CurrentItem()->ApplyAction(action);
                }
                else
                {
                    CurrentItem()->Open();
                }
            }
        }
        else if (action.key == Key::Cancel)
        {
            data->item->Close();
        }
    }
}


void Choice::ApplyAction(const Action &action) const
{
    if (action.key == Key::Menu)
    {
        Math::CircleIncrease<uint8>(data->value, 0, (uint8)(NumChoices() - 1));

        if (data->func_on_change)
        {
            data->func_on_change();
        }
    }
}


int Choice::NumChoices() const
{
    for (int i = 0; ; i++)
    {
        if (data->names[i] == nullptr)
        {
            return i;
        }
    }
}


void DateTime::ApplyAction(const Action &action) const
{
    if (action.key == Key::Menu)
    {
        (*data->field)++;

        if (*data->field == NumFields())
        {
            if (data->is_alarm)
            {
                if (data->is_time)
                {
                    gset.alarm.time.Hour = data->date_time->Hour;
                    gset.alarm.time.Minute = data->date_time->Minute;
                }
                else
                {
                    gset.alarm.time.Day = data->date_time->Day;
                    gset.alarm.time.Month = data->date_time->Month;
                    gset.alarm.time.Year = data->date_time->Year;
                }

                PCF8563::SetAlarm(&gset.alarm.time, gset.alarm.enabled != 0);
            }
            else
            {
                RTCDateTime current_time = PCF8563::GetDateTime();

                if (data->is_time)
                {
                    current_time.Hour = data->date_time->Hour;
                    current_time.Minute = data->date_time->Minute;
                }
                else
                {
                    current_time.Day = data->date_time->Day;
                    current_time.Month = data->date_time->Month;
                    current_time.Year = data->date_time->Year;
                }

                PCF8563::SetDateTime(&current_time);
            }

            data->item->Close();
        }
    }
    else if (action.key == Key::Cancel)
    {
        data->item->Close();
    }
    else if (action.key == Key::Up)
    {
        ChangeValueInCurrentField(+1);
    }
    else if (action.key == Key::Down)
    {
        ChangeValueInCurrentField(-1);
    }
}


void DateTime::ChangeValueInCurrentField(int delta) const
{
    int field = *data->field;

    if (field > NumFields() - 1)
    {
        return;
    }

    uint8 *refs[3] =
    {
        &data->date_time->Day,
        &data->date_time->Month,
        &data->date_time->Year
    };

    int min[3] = { 1, 1, 00 };
    int max[3] = { 31, 12, 99 };

    if (data->is_time)
    {
        refs[0] = &data->date_time->Hour;
        refs[1] = &data->date_time->Minute;
        min[0] = 0;
        min[1] = 0;
        max[0] = 23;
        max[1] = 59;
    }

    int value = *refs[field];

    Math::CircleChange(&value, min[field], max[field], (delta < 0) ? -1 : 1);

    *refs[field] = (uint8)value;
}


void Page::NextCurrentItem() const
{
    Math::CircleIncrease<int8>(data->current_item, 0, (int8)(NumItems() - 1));
}


void Page::PrevCurrentItem() const
{
    Math::CircleDecrease<int8>(data->current_item, 0, (int8)(NumItems() - 1));
}


int Page::NumItems() const
{
    for(int i = 0; ; i++)
    {
        if (data->items[i] == nullptr)
        {
            return i;
        }
    }
}


void Item::Open() const
{
    *data->opened = 1;

    if (IsDateTime())
    {
        const DataDateTime *_data = GetDateTime()->data;

        *_data->field = 0;

        if (_data->is_alarm)
        {
            *_data->date_time = gset.alarm.time;
        }
        else
        {
            *_data->date_time = PCF8563::GetDateTime();
        }
    }

    Menu::SetCurrentItem(this);
}

void Item::Close() const
{
    *data->opened = 0;

    if (data->keeper == nullptr)
    {
        Menu::Close();
    }
    else
    {
        Menu::SetCurrentItem(data->keeper);
    }
}
