// 2024/03/01 22:45:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display7735.h"
#include "Modules/ST7735/ST7735.h"
#include "Display/Font/Font.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Display/Text.h"
#include "Hardware/Timer.h"


template int Text<64>::Write(int x, int y) const;


namespace Display
{
    namespace Buffer
    {
        static uint8 buffer[SIZE];

        static int current_part = 0;                            // Эту часть сейчас отрисовываем

        static void Fill()
        {
            std::memset(buffer, 0xFF, SIZE);
        }

        uint8 *GetLine(int y)
        {
            return buffer + y * Display::WIDTH;
        }
    }

    static void BeginScene(int num_part);
    static void EndScene(int num_parts);

    static void Clear();
}


void Display::Clear()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);
        EndScene(i);
    }
}


void Display::Init()
{
#ifndef DEBUG

    ST7735::Init();

    Clear();

    Font::SetType(TypeFont::_5);
#endif
}


uint Display::TimeEnabled()
{
    return ST7735::TimeEnabled();
}


void Display::Update()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 1000)
    {
        return;
    }

    meter.Reset();

#ifndef DEBUG
    for (int i = 3; i < 4; i++)
    {
        BeginScene(i);      // 0 ms
        PAN3060::FuncDraw();
        EndScene(i);        // 68 ms
    }
#endif
}


void Display::BeginScene(int num_part)
{
    Buffer::current_part = num_part;

    Buffer::Fill();
}


void Display::EndScene(int num_parts)
{
    ST7735::Enable();

    ST7735::WriteBuffer(HEIGHT / NUMBER_PARTS_HEIGHT * num_parts);
}


void Rect::Fill(int x0, int y0) const
{
    for (int y = y0; y < y0 + height; y++)
    {
        HLine(width).Draw(x0, y);
    }
}


void Rect::Draw(int x, int y) const
{
    HLine(width).Draw(x, y);
    HLine(width).Draw(x, y + height - 1);
    VLine(height).Draw(x, y);
    VLine(height).Draw(x + width - 1, y);
}


void HLine::Draw(int x, int y) const
{
    for (int i = 0; i < width; i++)
    {
        Pixel().Set(x++, y);
    }
}


void VLine::Draw(int x, int y) const
{
    for (int i = 0; i < height; i++)
    {
        Pixel().Set(x, y++);
    }
}


void Pixel::Set(int x, int y) const
{
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

    Display::Buffer::buffer[y * Display::WIDTH + x] = 0x00;
}


template<int capacity>
int Text<capacity>::Write(int x, int y) const
{
    pchar pointer = text;

    while (*pointer)
    {
        x = Char(*pointer++).Write(x, y);
        x += Font::GetSize(); //-V1026
    }

    return x;
}
