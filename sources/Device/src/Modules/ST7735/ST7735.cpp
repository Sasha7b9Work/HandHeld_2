// 2024/03/01 22:47:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/ST7735/ST7735.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Display/Display.h"
#include <gd32e23x.h>


namespace ST7735
{
    namespace _SPI
    {
        static void Init();

        static void SendByte(uint8);
        static void SendUInt16(uint16);
    }

    static bool is_enabled = false;

    static PinOut pinDC_RS(GPIOB, GPIO_PIN_11);   // PB11 22
    static PinOut pinRES(GPIOB, GPIO_PIN_10);     // PB10 21
    static PinOut pinBKG(GPIOA, GPIO_PIN_4);      // PA4  14
    static PinOut pinON(GPIOB, GPIO_PIN_2);       // PB2  20

    static void SendData16(uint16 word)
    {
        pinDC_RS.ToHi();

        _SPI::SendUInt16(word);
    }

    void SetWindow(int startX, int startY, int stopX, int stopY);

    static void Write_Cmd(unsigned char CMD)
    {
        pinDC_RS.ToLow();

        _SPI::SendByte(CMD);
    }

    static void SendCommand(uint8 command)
    {
        Write_Cmd(command);
    }

    static void Write_Cmd_Data(unsigned char CMDP)
    {
        pinDC_RS.ToHi();

        _SPI::SendByte(CMDP);
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
    _SPI::Init();

    pinON.Init();
    pinDC_RS.Init();
    pinRES.Init();
    pinBKG.Init();

    pinON.ToLow();

//    pinSCL.ToLow();

    pinBKG.ToHi();

    pinRES.ToLow();
    pinDC_RS.ToLow();

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

#ifdef BOARD_NEW
    x0 += 0; x1 += 1; y0 += 24; y1 += 26;
#else
    x0 += 1; x1 += 1; y0 += 26; y1 += 26;
#endif

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

    pinDC_RS.ToHi();

    for (int y = 0; y < Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT; y++)
    {
        uint8 *points = Display::Buffer::GetLine(y);

        for (int i = 0; i < Display::WIDTH; i++)
        {
            _SPI::SendUInt16(Color::colors[*points++]);

//            SendData16(Color::colors[*points++]);
        }
    }
}


void ST7735::_SPI::Init()
{
    gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);

    spi_parameter_struct  spi_init_struct;
    spi_i2s_deinit(SPI0);
    spi_struct_para_init(&spi_init_struct);

    /* configure SPI0 parameter */
    spi_init_struct.trans_mode = SPI_TRANSMODE_BDTRANSMIT;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_2;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    spi_enable(SPI0);
}


void ST7735::_SPI::SendByte(uint8 byte)
{
    spi_i2s_data_transmit(SPI0, byte);
}


void ST7735::_SPI::SendUInt16(uint16 word)
{
    spi_i2s_data_transmit(SPI0, word);
}
