// 2024/03/01 22:45:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display7735.h"
#include "Modules/ST7735/ST7735.h"
#include "Display/Font/Font.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Display/Text.h"


template int Text<64>::Write(int x, int y, const Color &color) const;


namespace Display
{
    namespace Buffer
    {
        static uint8 buffer[SIZE];

        static uint crc[NUMBER_PARTS_HEIGHT] = { 0, 0, 0, 0, 0, 0, 0, 0 };

        static int current_part = 0;                            // Эту часть сейчас отрисовываем

        bool MatchesCRC(uint _crc)
        {
            return _crc == crc[current_part];
        }

        static void Fill(const Color &color)
        {
            std::memset(buffer, color.value, SIZE);
        }

        uint8 *GetLine(int y)
        {
            return buffer + y * Display::WIDTH;
        }
    }

    static void BeginScene(int num_part);
    static void DrawScene(int num_part);
    static void EndScene(int num_parts);

    static void BeginSceneDebug(int num_part);
    static void DrawSceneDebug(int num_part);
    static void EndSceneDebug(int num_part);

    static TimeMeterMS meter_begin_scene;
    static TimeMeterMS meter_draw;
    static TimeMeterMS meter_end_scene;
    static TimeMeterMS meter_full;

    static uint time_begin_scene = 0;
    static uint time_draw = 0;
    static uint time_end_scene = 0;
    static uint time_full = 0;
}


void Display::Init()
{
    ST7735::Init();

    Font::SetType(TypeFont::_7);
}


uint Display::TimeEnabled()
{
    return ST7735::TimeEnabled();
}


void Display::PrepareToSleep()
{
    ST7735::Disable();

    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        Buffer::crc[i] = 0;                         // Без этого не будет выходить по кнопке из сна
    }
}


void Display::Update()
{
}


void Display::DrawPowerOff()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("ВЫКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::DrawPowerOn()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("ВКЛЮЧЕНИЕ").WriteInCenter(0, 30, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::DrawLowVoltage()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);

        Font::SetSize(2);

        Text<>("НИЗКОЕ").WriteInCenter(0, 20, Display::WIDTH, Color::WHITE);

        Text<>("НАПРЯЖЕНИЕ").WriteInCenter(0, 50, Display::WIDTH, Color::WHITE);

        EndScene(i);
    }
}


void Display::BeginScene(int num_part)
{
    Buffer::current_part = num_part;

    Color color = Color::BLACK;

    Buffer::Fill(color);
}


void Display::EndScene(int num_parts)
{
    ST7735::Enable();

    ST7735::WriteBuffer(HEIGHT / NUMBER_PARTS_HEIGHT * num_parts);
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


void HLine::Draw(int x, int y, const Color &color) const
{
    color.SetAsCurrent();

    for (int i = 0; i < width; i++)
    {
        Pixel().Set(x++, y);
    }
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


void Display::BeginSceneDebug(int num_part)
{
    meter_begin_scene.Resume();

    Buffer::current_part = num_part;

    Buffer::Fill(Color::BLACK);

    meter_begin_scene.Pause();
}


void Display::DrawSceneDebug(int)
{
    meter_draw.Resume();

    int x = 10;
    int y = 10;
    int dy = 10;

    Text<64>("begin scene").Write(x, y, Color::WHITE);

    y += dy;

    Text<64>("draw scene").Write(x, y, Color::WHITE);

    y += dy;

    Text<64>("end scene").Write(x, y);

    y += dy;

    Text<64>("time full").Write(x, y);

    y += dy;

    Text<64>("sum time").Write(x, y);

    y += dy;

    static int counter = 0;

    if ((counter++ % 2) == 0)
    {
    }

    meter_draw.Pause();
}


void Display::EndSceneDebug(int num_part)
{
    meter_end_scene.Resume();

    ST7735::Enable();

    ST7735::WriteBuffer(HEIGHT / NUMBER_PARTS_HEIGHT * num_part);

    meter_end_scene.Pause();
}


void Display::UpdateDebug()
{
    meter_begin_scene.Reset();
    meter_begin_scene.Pause();

    meter_draw.Reset();
    meter_draw.Pause();

    meter_end_scene.Reset();
    meter_end_scene.Pause();

    meter_full.Reset();

    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginSceneDebug(i);
        DrawSceneDebug(i);
        EndSceneDebug(i);
    }

    time_begin_scene = meter_begin_scene.ElapsedTime();
    time_draw = meter_draw.ElapsedTime();
    time_end_scene = meter_end_scene.ElapsedTime();

    time_full = meter_full.ElapsedTime();
}
