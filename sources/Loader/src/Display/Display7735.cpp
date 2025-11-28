// 2024/03/01 22:45:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display7735.h"
#include "Modules/ST7735/ST7735.h"
#include "Display/Font/Font.h"
#include "Hardware/Keyboard.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/PAN3060/PAN3060.h"
#include "Display/Text.h"


template int Text<64>::Write(int x, int y, const Color &color) const;


namespace Display
{
    namespace Buffer
    {
        static uint8 buffer[SIZE];

        static int current_part = 0;                            // Эту часть сейчас отрисовываем

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
}


void Display::Init()
{
    ST7735::Init();

    Font::SetType(TypeFont::_5);
}


uint Display::TimeEnabled()
{
    return ST7735::TimeEnabled();
}


void Display::Update()
{
    for (int i = 0; i < NUMBER_PARTS_HEIGHT; i++)
    {
        BeginScene(i);      // 0 ms
        DrawScene(i);       // 20 ms
        EndScene(i);        // 68 ms
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


void Display::DrawScene(int /*num_part*/)
{
    Text<>("Обновление :").Write(10, 10, Color::WHITE);
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
