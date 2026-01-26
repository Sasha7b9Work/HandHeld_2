// 2026/01/26 13:47:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Display/Text.h"
#include "Settings/Source.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Keyboard/Keyboard.h"
#include "Utils/StringUtils.h"
#include "Menu/Menu.h"
#include "Utils/FPS.h"
#include "Hardware/Power.h"
#ifdef MODEL7735
    #include "Modules/ST7735/ST7735.h"
#endif
#ifdef MODEL7789
    #include "Modules/ST7789/ST7789.h"
#endif


template int Text<64>::Write(int x, int y, const Color &color) const;


namespace Display
{
    namespace Buffer
    {
        uint8 buffer[SIZE];

        int current_part = 0;                            // Ýòó ÷àñòü ñåé÷àñ îòðèñîâûâàåì

        uint8 *GetLine(int y)
        {
            return buffer + y * Display::WIDTH;
        }

        static void Fill(const Color &color)
        {
            std::memset(buffer, color.value, SIZE);
        }
    }

    void BeginScene(int num_part);
    static void DrawScene(int num_part);
    extern void EndScene(int num_parts);
}


template<int capacity>
int Text<capacity>::Write(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    pchar pointer = text;

    while (*pointer)
    {
        x = Char(*pointer++).Write(x, y);
        x += Font::GetSize(); //-V1026
    }

    return x;
}


void Rect::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    HLine(width).Draw(x, y);
    HLine(width).Draw(x, y + height - 1);
    VLine(height).Draw(x, y);
    VLine(height).Draw(x + width - 1, y);
}


void VLine::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    for (int i = 0; i < height; i++)
    {
        Pixel().Set(x, y++);
    }
}


void Pixel::Set(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    if (x < 0)
    {
        return;
    }

    if (x >= Display::WIDTH)
    {
        return;
    }

    y -= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT * Display::Buffer::current_part;

    if (y < 0)
    {
        return;
    }

    if (y >= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT)
    {
        return;
    }

    Display::Buffer::buffer[y * Display::WIDTH + x] = (uint8)Color::current.value;
}


void HLine::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    if (x >= Display::WIDTH)
    {
        return;
    }

    y -= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT * Display::Buffer::current_part;

    if (y < 0)
    {
        return;
    }

    if (y >= Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT)
    {
        return;
    }

    uint8 *pixel = Display::Buffer::buffer + y * Display::WIDTH + x;

    for (int i = 0; i < width; i++)
    {
        *pixel++ = (uint8)Color::current.value;
    }
}


void Rect::Fill(int x0, int y0, const Color &color) const
{
    color.SetAsCurrent();

    for (int y = y0; y < y0 + height; y++)
    {
        HLine(width).Draw(x0, y);
    }
}


void Display::DrawPowerOff()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("ÂÛÊËÞ×ÅÍÈÅ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::BeginScene(int num_part)
{
    Buffer::current_part = num_part;

    Color color = Color::BLACK;

    if (Source::GetCountReceived())
    {
        color = gset.sources[Source::Current()].color;
    }

    if (PCF8563::IsAlarmed())
    {
        color = gset.alarm.color;
    }

    Buffer::Fill(color);
}


void Display::Update()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 50)
    {
        return;
    }

    meter.Reset();

    ModeClock::Set(ModeClock::Hi);

    if (PCF8563::IsAlarmed() || Source::GetCountReceived() || !Keyboard::ToMoreTime())
    {
        FPS::BeginFrame();

        for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
        {
            BeginScene(i);      // 0 ms
            DrawScene(i);       // 20 ms
            EndScene(i);        // 68 ms
        }

        FPS::EndFrame();
    }

    if (Source::GetCountReceived() == 0 && !PCF8563::IsAlarmed())
    {
        ModeClock::Set(ModeClock::Low);
    }
}


void Display::DrawScene(int num_part)
{
    (void)num_part;

    if (PCF8563::IsAlarmed())
    {
        Font::SetSize(2);
        Text<>("ÁÓÄÈËÜÍÈÊ").WriteInCenter(0, 30, Display::WIDTH, Color(Color::Contrast(gset.alarm.color)));
        Font::SetSize(1);
    }
    else if (Source::GetCountReceived())
    {
        Color color = Color::Contrast(gset.sources[Source::Current()].color);

        color.SetAsCurrent();

        int y = 40;

        if (Source::GetCountReceived() == 1)
        {
            y = 30;
        }
        else
        {
            for (int i = 0; i < Source::Count; i++)
            {
                if (Source::IsReceived((Source::E)i))
                {
                    Source((Source::E)i).DrawIcon(11 + i * 30, 8);
                }
            }
        }

        Font::SetSize(2);

        pchar name = Source::Name(Source::Current());

        int num_words = SU::NumWordsInString(name);

        if (num_words == 1)
        {
            Text<>(name).WriteInCenter(0, y, Display::WIDTH);
        }
        else if (num_words == 2)
        {
            char buffer[32];

            Text<>(SU::GetWordFromString(name, 1, buffer)).WriteInCenter(0, y - 10, Display::WIDTH);

            Text<>(SU::GetWordFromString(name, 2, buffer)).WriteInCenter(0, y + 15, Display::WIDTH);
        }

        Font::SetSize(1);
    }
    else
    {
        if (Menu::IsShown())
        {
            Menu::Draw();
        }
        else
        {
            Font::SetSize(5);

            PCF8563::GetDateTime().DrawTime(30, 23, Color::WHITE);

            Font::SetSize(2);

            PCF8563::GetDateTime().DrawDate(46, 66);

            Font::SetSize(1);

            Power::Draw();
        }
    }
}


void RTCDateTime::DrawTime(int x, int y, const Color &color) const
{
    Text<>("%02d:%02d", Hour, Minute).Write(x, y, color);
}


void RTCDateTime::DrawDate(int x, int y, const Color &color) const
{
    Text<>("%02d/%02d/%02d", Day, Month, Year).Write(x, y, color);
}


uint Display::TimeEnabled()
{
#ifdef MODEL7735
    return ST7735::TimeEnabled();
#endif

#ifdef MODEL7789
    return ST7789::TimeEnabled();
#endif
}
