// 2024/03/01 22:47:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/ST7735/ST7735.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Display/Display7735.h"
#include <gd32e23x.h>


namespace ST7735
{
    static bool is_enabled = false;

    static PinOut pinDC(GPIOB, GPIO_PIN_11);      // PB11 22
    static PinOut pinRES(GPIOB, GPIO_PIN_10);     // PB10 21
    static PinOut pinBKG(GPIOA, GPIO_PIN_4);      // PA4  14
    static PinOut pinSCL(GPIOA, GPIO_PIN_5);      // PA5  15
    static PinOut pinSDA(GPIOA, GPIO_PIN_7);      // PA7  17
    static PinOut pinOn(GPIOB, GPIO_PIN_2);       // PB2  20

#define SDA_TO_LOW GPIO_BC(GPIOA) = (uint32_t)GPIO_PIN_7
#define SDA_TO_HI  GPIO_BOP(GPIOA) = (uint32_t)GPIO_PIN_7

#define SCL_TO_HI  GPIO_BOP(GPIOA) = (uint32_t)GPIO_PIN_5
#define SCL_TO_LOW GPIO_BC(GPIOA) = (uint32_t)GPIO_PIN_5

#define ONE_CLOCK   \
        SCL_TO_HI;  \
        SCL_TO_LOW


    static void SendByte(uint8 byte)
    {
        if ((byte & 0x80) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x40) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x20) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x10) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x08) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x04) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x02) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;

        if ((byte & 0x01) == 0)
        {
            SDA_TO_LOW;
        }
        else
        {
            SDA_TO_HI;
        }

        ONE_CLOCK;
    }


    static void SendData16(uint16 word)
    {
        pinDC.ToHi();

        SendByte((uint8)(word >> 8));
        SendByte((uint8)word);
    }

    void SetWindow(int startX, int startY, int stopX, int stopY);

    static void Write_Cmd(unsigned char CMD)
    {
        pinDC.ToLow();

        SendByte(CMD);
    }

    static void SendCommand(uint8 command)
    {
        Write_Cmd(command);
    }

    static void Write_Cmd_Data(unsigned char CMDP)
    {
        pinDC.ToHi();

        SendByte(CMDP);
    }

    static void SendData8(uint8 byte)
    {
        Write_Cmd_Data(byte);
    }

    static void LCD_SetPos_Vertical(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1);

    static void LCD_SetPos_Horizontal(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1);

    static uint time_enable = 0;
}


void ST7735::Enable()
{
    if (is_enabled)
    {
        return;
    }

    time_enable = TIME_MS;

    Write_Cmd(0x11);            // Sleep Out

    pinBKG.ToHi();

    is_enabled = true;
}


void ST7735::Disable()
{
    if (!is_enabled)
    {
        return;
    }

    pinBKG.ToLow();

    Write_Cmd(0x10);            // Sleep In

    is_enabled = false;
}


bool ST7735::IsEnabled()
{
    return is_enabled;
}


uint ST7735::TimeEnabled()
{
    return TIME_MS - time_enable;
}


void ST7735::Init()
{
    pinOn.Init();
    pinDC.Init();
    pinRES.Init();
    pinBKG.Init();
    pinSCL.Init();
    pinSDA.Init();

    pinOn.ToHi();

    pinSCL.ToLow();

    pinBKG.ToHi();

    pinRES.ToLow();
    pinDC.ToLow();

    pinRES.ToLow(); //-V525
    Timer::Delay(50);
    pinRES.ToHi();
    Timer::Delay(150);

    Write_Cmd(0x11);

    Timer::Delay(120);

    Write_Cmd(0x21);                // Display Inversion On

    Write_Cmd(0xB1);                // Frame Rate Control (In normal mode/ Full colors)
    Write_Cmd_Data(0x05);
    Write_Cmd_Data(0x3A);
    Write_Cmd_Data(0x3A);

    Write_Cmd(0xB2);                // Frame Rate Control (In Idle mode/ 8-colors) 
    Write_Cmd_Data(0x05);
    Write_Cmd_Data(0x3A);
    Write_Cmd_Data(0x3A);

    Write_Cmd(0xB3);                // Frame Rate Control (In Partial mode/ full colors)
    Write_Cmd_Data(0x05);
    Write_Cmd_Data(0x3A);
    Write_Cmd_Data(0x3A);
    Write_Cmd_Data(0x05);
    Write_Cmd_Data(0x3A);
    Write_Cmd_Data(0x3A);

    Write_Cmd(0xB4);                // Display Inversion Control
    Write_Cmd_Data(0x03);

    Write_Cmd(0xC0);                // Power Control 1 
    Write_Cmd_Data(0x62);
    Write_Cmd_Data(0x02);
    Write_Cmd_Data(0x04);

    Write_Cmd(0xC1);                //  Power Control 2 
    Write_Cmd_Data(0xC0);

    Write_Cmd(0xC2);                // Power Control 3 (in Normal mode/ Full colors)
    Write_Cmd_Data(0x0D);
    Write_Cmd_Data(0x00);

    Write_Cmd(0xC3);                // Power Control 4 (in Idle mode/ 8-colors)
    Write_Cmd_Data(0x8D);
    Write_Cmd_Data(0x6A);

    Write_Cmd(0xC4);                // Power Control 5 (in Partial mode/ full-colors)
    Write_Cmd_Data(0x8D);
    Write_Cmd_Data(0xEE);

    Write_Cmd(0xC5);                // VCOM Control 1
    Write_Cmd_Data(0x0E);

    Write_Cmd(0xE0);                //  Gamma (‘+’polarity) Correction Characteristics Setting
    Write_Cmd_Data(0x10);
    Write_Cmd_Data(0x0E);
    Write_Cmd_Data(0x02);
    Write_Cmd_Data(0x03);
    Write_Cmd_Data(0x0E);
    Write_Cmd_Data(0x07);
    Write_Cmd_Data(0x02);
    Write_Cmd_Data(0x07);
    Write_Cmd_Data(0x0A);
    Write_Cmd_Data(0x12);
    Write_Cmd_Data(0x27);
    Write_Cmd_Data(0x37);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(0x0D);
    Write_Cmd_Data(0x0E);
    Write_Cmd_Data(0x10);


    Write_Cmd(0xE1);                // Gamma ‘-’polarity Correction Characteristics Setting
    Write_Cmd_Data(0x10);
    Write_Cmd_Data(0x0E);
    Write_Cmd_Data(0x03);
    Write_Cmd_Data(0x03);
    Write_Cmd_Data(0x0F);
    Write_Cmd_Data(0x06);
    Write_Cmd_Data(0x02);
    Write_Cmd_Data(0x08);
    Write_Cmd_Data(0x0A);
    Write_Cmd_Data(0x13);
    Write_Cmd_Data(0x26);
    Write_Cmd_Data(0x36);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(0x0D);
    Write_Cmd_Data(0x0E);
    Write_Cmd_Data(0x10);

    Write_Cmd(0x3A);                // Interface Pixel Format
    Write_Cmd_Data(0x05);

    Write_Cmd(0x36);                // Memory Data Access Control
    Write_Cmd_Data(0x68);

    Write_Cmd(0x29);                // Display On

    Write_Cmd(0x2C);                // Memory Write

    is_enabled = true;
}


void ST7735::LCD_SetPos_Vertical(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1)
{
    unsigned char YSH, YSL, YEH, YEL;
    x0 += 0; x1 += 26; y0 += 26; y1 += 26;

    YSH = (uint8)(y0 >> 8);
    YSL = (uint8)y0;

    YEH = (uint8)(y1 >> 8);
    YEL = (uint8)y1;

    Write_Cmd(0x2A);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(x0);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(x1);
    Write_Cmd(0x2B);
    Write_Cmd_Data(YSH);
    Write_Cmd_Data(YSL);
    Write_Cmd_Data(YEH);
    Write_Cmd_Data(YEL);
    Write_Cmd(0x2C);//LCD_WriteCMD(GRAMWR);
}


void ST7735::SetWindow(int x, int y, int width, int height)
{
    SendCommand(0x2A);      // CASET
    SendData8(0x00);
    SendData8((uint8)x);
    SendData8(0x00);
    SendData8((uint8)(x + width - 1));
    SendCommand(0x2B);      // RASET
    SendData8(0x00);
    SendData8((uint8)y);
    SendData8(0x00);
    SendData8((uint8)(y + height));
}


void ST7735::Fill(uint16 color)
{
    LCD_SetPos_Vertical(0, 159, 0, 79);

    for (uint w = 0; w < 160; w++)
    {
        for (uint u = 0; u < 80; u++)
        {
            SendData16(color);
        }
    }
}


void ST7735::LCD_SetPos_Horizontal(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1)
{
    unsigned char YSH, YSL, YEH, YEL;
    x0 += 1; x1 += 1; y0 += 26; y1 += 26;

    YSH = (uint8)(y0 >> 8);
    YSL = (uint8)y0;

    YEH = (uint8)(y1 >> 8);
    YEL = (uint8)y1;

    Write_Cmd(0x2A);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(x0);
    Write_Cmd_Data(0x00);
    Write_Cmd_Data(x1);
    Write_Cmd(0x2B);
    Write_Cmd_Data(YSH);
    Write_Cmd_Data(YSL);
    Write_Cmd_Data(YEH);
    Write_Cmd_Data(YEL);
    Write_Cmd(0x2C);//LCD_WriteCMD(GRAMWR);
}


void ST7735::WriteBuffer(int y0)
{
    LCD_SetPos_Horizontal(0, Display::WIDTH - 1, (uint)y0, (uint)(y0 + Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT - 1));

    for (int y = 0; y < Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT; y++)
    {
        uint8 *points = Display::Buffer::GetLine(y);

        for (int i = 0; i < Display::WIDTH; i++)
        {
            SendData16(Color::colors[*points++]);
        }
    }
}
