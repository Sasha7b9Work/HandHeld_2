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
#include "Display/Primitives.h"
#include "Display/Font/Font.h"


namespace Display
{
    namespace Buffer
    {
        uint8 buffer[SIZE] __attribute__((aligned(4))) = { 0 };

        static uint crc[NUMBER_PARTS_HEIGHT] = { 0, 0, 0, 0, 0, 0, 0, 0 };

        int current_part = 0;                                   // Эту часть сейчас отрисовываем

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
    }

    static void BeginScene();
    static void DrawScene();
    static void EndScene();

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

    Font::SetMainType();

    {
        // Читаем байт конфигурации из области загрузчика чтобы правильно работать с дисплеем

        if (HAL_ROM::ReadByte(0x8001FFF) == 0x00)
        {
            old_display = true;
        }
    }

    Color::Init();

    for (Buffer::current_part = 0; Buffer::current_part < NUMBER_PARTS_HEIGHT; Buffer::current_part++)
    {
        BeginScene();
        EndScene();
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

        for (Buffer::current_part = 0; Buffer::current_part < NUMBER_PARTS_HEIGHT; Buffer::current_part++)
        {
            BeginScene();
            DrawScene();
            EndScene();
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
    for (Buffer::current_part = 0; Buffer::current_part < NUMBER_PARTS_HEIGHT; Buffer::current_part++)
    {
        BeginScene();

        Font::SetSize(2);

        Text<>("ВЫКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene();
    }
}


void Display::DrawPowerOn()
{
    for (Buffer::current_part = 0; Buffer::current_part < NUMBER_PARTS_HEIGHT; Buffer::current_part++)
    {
        BeginScene();

        Font::SetSize(2);

        Text<>("ВКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene();
    }
}


void Display::DrawLowVoltage()
{
    for (Buffer::current_part = 0; Buffer::current_part < NUMBER_PARTS_HEIGHT; Buffer::current_part++)
    {
        BeginScene();

        Font::SetSize(2);

        Text<>("НИЗКОЕ").WriteInCenter(0, 20, Display::WIDTH, Color::RED);

        Text<>("НАПРЯЖЕНИЕ").WriteInCenter(0, 50, Display::WIDTH);

        EndScene();
    }
}


void Display::BeginScene()
{
    Color color = Color::Background();

    if (PCF8563::IsAlarmed())
    {
        color = gset.alarm.color;
    }

    Buffer::Fill(color);
}


void Display::EndScene()
{
    uint crc = Buffer::CalcualteCRC();

#ifndef MODEL7789
    if (crc != Buffer::crc[Buffer::current_part])
#endif
    {
        ST7735_89::Enable();

        Buffer::crc[Buffer::current_part] = crc;

        ST7735_89::WriteBuffer(Buffer::current_part);
    }
}


void Display::DrawScene()
{
    if (PCF8563::IsAlarmed())
    {
        Font::SetSize(2);
        Text<>("БУДИЛЬНИК").WriteInCenter(0, 30, Display::WIDTH, Color(Color::Contrast(gset.alarm.color)));
        Font::SetSize(1);
    }
    else if (Source::GetCountReceived())
    {
#ifdef MODEL7789
        int offset_y = Source::GetCountReceived() > 1 ? 40 : 0;
#else
        int offset_y = Source::GetCountReceived() > 1 ? 10 : 0;
#endif

        Color::Draw().SetAsCurrent();

        if (Source::GetCountReceived() > 1)
        {
            Font::StoreType();
            Font::SetMiddleType();

            for (int i = 0; i < Source::Count; i++)
            {
                if (Source::IsReceived((Source::E)i))
                {
                    Source((Source::E)i).DrawIcon(i);
                }
            }

            Font::RestoreType();
        }

        Display::DrawTitleOnFullScreen(Text<>(Source::Name(Source::Current())), offset_y);
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

            PCF8563::GetDateTime().DrawTime(Color::Draw());

            Font::SetSize(2);

            PCF8563::GetDateTime().DrawDate();

            Font::SetSize(1);

            Power::Draw();
        }
    }
}


void Display::DrawTitleOnFullScreen(const Text<> &title, int offset_y)
{
    Font::SetSize(2);

    int num_words = SU::NumWordsInString(title.c_str());

    if (num_words == 1)
    {
        title.WriteInCenter(0, SU::Y::Center() + offset_y, Display::WIDTH);
    }
    else if (num_words == 2)
    {
        char buffer[32];

        Text<>(SU::GetWordFromString(title.c_str(), 1, buffer)).WriteInCenter(0, SU::Y::Up() + offset_y, Display::WIDTH);

        Text<>(SU::GetWordFromString(title.c_str(), 2, buffer)).WriteInCenter(0, SU::Y::Down() + offset_y, Display::WIDTH);
    }

    Font::SetSize(1);
}
