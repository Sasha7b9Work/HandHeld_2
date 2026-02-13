// 2024/03/01 22:45:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Modules/ST7735_89/ST7735_89.h"
#include "Settings/Settings.h"
#include "Display/Font/Font.h"
#include "Keyboard/Keyboard.h"
#include "Hardware/HAL/HAL.h"
#include "Menu/Menu.h"
#include "Utils/StringUtils.h"
#include "Hardware/Power.h"
#include "Utils/Math.h"
#include "Modules/PCF8563/PCF8563.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"
#include "Utils/FPS.h"


template int Text<64>::Write(int x, int y, const Color &color) const;


namespace Display
{
    namespace Buffer
    {
        static uint8 buffer[SIZE];

        static uint crc[NUMBER_PARTS_HEIGHT] = { 0, 0, 0, 0, 0, 0, 0, 0 };

        static int current_part = 0;                            // Эту часть сейчас отрисовываем

        static uint CalcualteCRC()
        {
            return Math::CalculateCRC32(buffer, SIZE);
        }

        static Color color_last_filled(Color::BLACK);           // Этим цветом шло последнее заполнение

        static void Fill(const Color &color)
        {
            color_last_filled = color;

            std::memset(buffer, color.value, SIZE);
        }

        uint8 *GetLine(int y)
        {
            return buffer + y * Display::WIDTH;
        }

        uint8 *GetShiftedLine(int y)
        {
            static uint8 buffer_line[Display::WIDTH];

            int null_points = 80;

            std::memset(buffer_line, color_last_filled.value, (size_t)null_points);

            std::memcpy(buffer_line + null_points, GetLine(y), (uint)(Display::WIDTH - null_points));

            return buffer_line;
        }
    }

    static void BeginScene(int num_part);
    static void DrawScene(int num_part);
    static void EndScene(int num_parts);

    static bool old_display = false;    // Если true, то используется старый тип дисплея - без синей полосы с левого края экрана.
                                        // Это значение зависит от байта в загрузчике
}


bool Display::IsOldType()
{
    return old_display;
}


void Display::Init()
{
    ST7735_89::Init();

    Font::SetType(TypeFont::_7);

    {
        // Читаем байт конфигурации из области загрузчика чтобы правильно работать с дисплеем

#define _FLASH_ADDRESS 0x8001FFF

        uint8 value = *(volatile uint8 *)_FLASH_ADDRESS;

        if (value == 0x00)
        {
            old_display = true;
        }
    }

    Color::Init();

    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);
        EndScene(i);
    }

    ST7735_89::Disable();

    ST7735_89::Enable();
}


uint Display::TimeEnabled()
{
    return ST7735_89::TimeEnabled();
}


void Display::PrepareToSleep()
{
    ST7735_89::Disable();

    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        Buffer::crc[i] = 0;                         // Без этого не будет выходить по кнопке из сна
    }
}


void Display::Refresh()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        Buffer::crc[i] = 0;
    }
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
            BeginScene(i);
            DrawScene(i);
            EndScene(i);
        }

        FPS::EndFrame();
    }

    if (Source::GetCountReceived() == 0 && !PCF8563::IsAlarmed())
    {
        ModeClock::Set(ModeClock::Low);
    }
}


void Display::DrawPowerOff()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(S_2_3);

        Text<>("ВЫКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::DrawPowerOn()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(S_2_3);

        Text<>("ВКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::DrawLowVoltage()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(S_2_3);

        Text<>("НИЗКОЕ").WriteInCenter(0, 20, Display::WIDTH, Color::RED);

        Text<>("НАПРЯЖЕНИЕ").WriteInCenter(0, 50, Display::WIDTH, Color::RED);

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


void Display::EndScene(int num_parts)
{
    uint crc = Buffer::CalcualteCRC();

#ifndef MODEL7789
    if (crc != Buffer::crc[Buffer::current_part])
#endif
    {
        ST7735_89::Enable();

        Buffer::crc[Buffer::current_part] = crc;

//        ST7735_89::_WriteBuffer(HEIGHT / NUMBER_PARTS_HEIGHT * num_parts);

        ST7735_89::WriteBuffer(num_parts);
    }
}


void Display::DrawScene(int num_part)
{
    if (PCF8563::IsAlarmed())
    {
        Font::SetSize(S_2_3);
        Text<>("БУДИЛЬНИК").WriteInCenter(0, 30, Display::WIDTH, Color(Color::Contrast(gset.alarm.color)));
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

        Font::SetSize(S_2_3);

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
            Font::SetSize(S_5_6);

            PCF8563::GetDateTime().DrawTime(30, 23, Color::WHITE);

            Font::SetSize(S_2_3);

            PCF8563::GetDateTime().DrawDate(46, 66);

            Font::SetSize(1);

            if (num_part == 0)
            {
                Power::Draw();
            }
        }
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


void RTCDateTime::DrawTime(int x, int y, const Color &color) const
{
    Text<>("%02d:%02d", Hour, Minute).Write(x, y, color);
}


void RTCDateTime::DrawDate(int x, int y, const Color &color) const
{
    Text<>("%02d/%02d/%02d", Day, Month, Year).Write(x, y, color);
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
