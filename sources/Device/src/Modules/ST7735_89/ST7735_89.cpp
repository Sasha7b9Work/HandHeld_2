// 2024/03/01 22:47:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/ST7735_89/ST7735_89.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/Timer.h"
#include "Display/Display.h"
#include "Utils/FPS.h"
#include "system.h"


namespace ST7735_89
{
    /*
    *   TFT_SCK - 15 : PA5     AF0 SPI0_SCK
    *   TFT_SDA - 17 : PA7     AF0 SPI0_MOSI
    */
    static bool is_enabled = false;

    static PinOut pinDC_RS(GPIOB, GPIO_PIN_11);   // PB11 22
    static PinOut pinRES(GPIOB, GPIO_PIN_10);     // PB10 21
    static PinOut pinBKG(GPIOA, GPIO_PIN_4);      // PA4  14
    static PinOut pinON(GPIOB, GPIO_PIN_2);       // PB2  20

    static void SendByte(uint8 byte)
    {
        spi_i2s_data_transmit(SPI0, byte);
    }


    static void Write_Cmd(unsigned char CMD)
    {
        pinDC_RS.ToLow();

        SendByte(CMD);
    }

    static void Write_Data(unsigned char CMDP)
    {
        pinDC_RS.ToHi();

        SendByte(CMDP);
    }

    static void LCD_SetPos_Horizontal(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1);

    static uint time_enable = 0;
}


void ST7735_89::Enable()
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


void ST7735_89::Disable()
{
    if (!is_enabled)
    {
        return;
    }

    pinBKG.ToLow();

    Write_Cmd(0x10);            // Sleep In

    is_enabled = false;
}


bool ST7735_89::IsEnabled()
{
    return is_enabled;
}


uint ST7735_89::TimeEnabled()
{
    return TIME_MS - time_enable;
}


void ST7735_89::Init()
{
    {
#ifdef MODEL7735
        gpio_af_set(GPIOA, GPIO_AF_0, GPIO_PIN_5 | GPIO_PIN_7);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_5 | GPIO_PIN_7);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
#endif

#ifdef MODEL7789
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
#endif
    }

    {
        spi_parameter_struct spi_is;
        spi_i2s_deinit(SPI0);
        spi_struct_para_init(&spi_is);

        spi_is.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
        spi_is.device_mode = SPI_MASTER;
        spi_is.frame_size = SPI_FRAMESIZE_8BIT;
        spi_is.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
        spi_is.nss = SPI_NSS_SOFT;
        spi_is.prescale = SPI_PSC_2;
        spi_is.endian = SPI_ENDIAN_MSB;
        spi_init(SPI0, &spi_is);
    }

    spi_enable(SPI0);

    pinON.Init();
    pinDC_RS.Init();
    pinRES.Init();
    pinBKG.Init();

    pinON.ToLow();

    pinBKG.ToHi();

    pinRES.ToLow();
    pinDC_RS.ToLow();

    pinRES.ToLow(); //-V525
    Timer::Delay(50);
    pinRES.ToHi();
    Timer::Delay(150);

    Write_Cmd(0x11);                // Sleep Out

    Timer::Delay(120);

#ifdef MODEL7735

    Write_Cmd(0x21);                // Display Inversion On

    Write_Cmd(0xB1);                // Frame Rate Control (In normal mode/ Full colors)
    Write_Data(0x05);
    Write_Data(0x3A);
    Write_Data(0x3A);

    Write_Cmd(0xB2);                // Frame Rate Control (In Idle mode/ 8-colors) 
    Write_Data(0x05);
    Write_Data(0x3A);
    Write_Data(0x3A);

    Write_Cmd(0xB3);                // Frame Rate Control (In Partial mode/ full colors)
    Write_Data(0x05);
    Write_Data(0x3A);
    Write_Data(0x3A);
    Write_Data(0x05);
    Write_Data(0x3A);
    Write_Data(0x3A);

    Write_Cmd(0xB4);                // Display Inversion Control
    Write_Data(0x03);

    Write_Cmd(0xC0);                // Power Control 1 
    Write_Data(0x62);
    Write_Data(0x02);
    Write_Data(0x04);

    Write_Cmd(0xC1);                //  Power Control 2 
    Write_Data(0xC0);

    Write_Cmd(0xC2);                // Power Control 3 (in Normal mode/ Full colors)
    Write_Data(0x0D);
    Write_Data(0x00);

    Write_Cmd(0xC3);                // Power Control 4 (in Idle mode/ 8-colors)
    Write_Data(0x8D);
    Write_Data(0x6A);

    Write_Cmd(0xC4);                // Power Control 5 (in Partial mode/ full-colors)
    Write_Data(0x8D);
    Write_Data(0xEE);

    Write_Cmd(0xC5);                // VCOM Control 1
    Write_Data(0x0E);

    Write_Cmd(0xE0);                //  Gamma (‘+’polarity) Correction Characteristics Setting
    Write_Data(0x10);
    Write_Data(0x0E);
    Write_Data(0x02);
    Write_Data(0x03);
    Write_Data(0x0E);
    Write_Data(0x07);
    Write_Data(0x02);
    Write_Data(0x07);
    Write_Data(0x0A);
    Write_Data(0x12);
    Write_Data(0x27);
    Write_Data(0x37);
    Write_Data(0x00);
    Write_Data(0x0D);
    Write_Data(0x0E);
    Write_Data(0x10);


    Write_Cmd(0xE1);                // Gamma ‘-’polarity Correction Characteristics Setting
    Write_Data(0x10);
    Write_Data(0x0E);
    Write_Data(0x03);
    Write_Data(0x03);
    Write_Data(0x0F);
    Write_Data(0x06);
    Write_Data(0x02);
    Write_Data(0x08);
    Write_Data(0x0A);
    Write_Data(0x13);
    Write_Data(0x26);
    Write_Data(0x36);
    Write_Data(0x00);
    Write_Data(0x0D);
    Write_Data(0x0E);
    Write_Data(0x10);

    Write_Cmd(0x3A);                // Interface Pixel Format
    Write_Data(0x05);

    Write_Cmd(0x36);                // Memory Data Access Control
    Write_Data(0x68);

    Write_Cmd(0x29);                // Display On

    Write_Cmd(0x2C);                // Memory Write

#endif

#ifdef MODEL7789

#define ST7789_NORON		0x13
#define ST7789_MADCTL		0x36      // Memory data access control
#define ST7789_RAMCTRL		0xB0      // RAM control
#define ST7789_COLMOD		0x3A
#define ST7789_PORCTRL		0xB2      // Porch control
#define ST7789_GCTRL		0xB7      // Gate control
#define ST7789_VCOMS		0xBB      // VCOMS setting
#define ST7789_LCMCTRL		0xC0      // LCM control
#define ST7789_VDVVRHEN		0xC2      // VDV and VRH command enable
#define ST7789_VRHS			0xC3      // VRH set
#define ST7789_VDVSET		0xC4      // VDV setting
#define ST7789_FRCTR2		0xC6      // FR Control 2
#define ST7789_PWCTRL1		0xD0      // Power control 1
#define ST7789_PVGAMCTRL	0xE0      // Positive voltage gamma control
#define ST7789_NVGAMCTRL	0xE1      // Negative voltage gamma control
#define ST7789_INVON		0x21
#define ST7789_CASET		0x2A
#define ST7789_RASET		0x2B
#define ST7789_DISPON		0x29

#define TFT_MAD_RGB         0x00
#define TFT_MAD_COLOR_ORDER TFT_MAD_RGB

    Write_Cmd(ST7789_NORON);    // Normal display mode on

    //------------------------------display and color format setting--------------------------------//
    Write_Cmd(ST7789_MADCTL);
    //writedata(0x00);
    Write_Data(TFT_MAD_COLOR_ORDER);

    // JLX240 display datasheet
    Write_Cmd(0xB6);
    Write_Data(0x0A);
    Write_Data(0x82);

    Write_Cmd(ST7789_RAMCTRL);
    Write_Data(0x00);
    Write_Data(0xE0); // 5 to 6-bit conversion: r0 = r5, b0 = b5

    Write_Cmd(ST7789_COLMOD);
    Write_Data(0x55);
    Timer::Delay(10);

    //--------------------------------ST7789V Frame rate setting----------------------------------//
    Write_Cmd(ST7789_PORCTRL);
    Write_Data(0x0c);
    Write_Data(0x0c);
    Write_Data(0x00);
    Write_Data(0x33);
    Write_Data(0x33);

    Write_Cmd(ST7789_GCTRL);      // Voltages: VGH / VGL
    Write_Data(0x35);

    //---------------------------------ST7789V Power setting--------------------------------------//
    Write_Cmd(ST7789_VCOMS);
    Write_Data(0x28);		// JLX240 display datasheet

    Write_Cmd(ST7789_LCMCTRL);
    Write_Data(0x0C);

    Write_Cmd(ST7789_VDVVRHEN);
    Write_Data(0x01);
    Write_Data(0xFF);

    Write_Cmd(ST7789_VRHS);       // voltage VRHS
    Write_Data(0x10);

    Write_Cmd(ST7789_VDVSET);
    Write_Data(0x20);

    Write_Cmd(ST7789_FRCTR2);
    Write_Data(0x0f);

    Write_Cmd(ST7789_PWCTRL1);
    Write_Data(0xa4);
    Write_Data(0xa1);

    //--------------------------------ST7789V gamma setting---------------------------------------//
    Write_Cmd(ST7789_PVGAMCTRL);
    Write_Data(0xd0);
    Write_Data(0x00);
    Write_Data(0x02);
    Write_Data(0x07);
    Write_Data(0x0a);
    Write_Data(0x28);
    Write_Data(0x32);
    Write_Data(0x44);
    Write_Data(0x42);
    Write_Data(0x06);
    Write_Data(0x0e);
    Write_Data(0x12);
    Write_Data(0x14);
    Write_Data(0x17);

    Write_Cmd(ST7789_NVGAMCTRL);
    Write_Data(0xd0);
    Write_Data(0x00);
    Write_Data(0x02);
    Write_Data(0x07);
    Write_Data(0x0a);
    Write_Data(0x28);
    Write_Data(0x31);
    Write_Data(0x54);
    Write_Data(0x47);
    Write_Data(0x0e);
    Write_Data(0x1c);
    Write_Data(0x17);
    Write_Data(0x1b);
    Write_Data(0x1e);

    Write_Cmd(ST7789_INVON);

    Write_Cmd(ST7789_CASET);    // Column address set
    Write_Data(0x00);
    Write_Data(0x00);
    Write_Data(0x00);
    Write_Data(0xEF);    // 239

    Write_Cmd(ST7789_RASET);    // Row address set
    Write_Data(0x00);
    Write_Data(0x00);
    Write_Data(0x01);
    Write_Data(0x3F);    // 319

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    end_tft_write();
    Timer::Delay(120);
//    begin_tft_write();

    Write_Cmd(ST7789_DISPON);    //Display on
    Timer::Delay(120);

#endif

    is_enabled = true;
}


void ST7735_89::LCD_SetPos_Horizontal(unsigned char x0, unsigned char x1, unsigned int y0, unsigned int y1)
{
    unsigned char YSH, YSL, YEH, YEL;

#ifdef MODEL7735
    if (Display::IsOldType())
    {
        x0 += 1; x1 += 1; y0 += 26; y1 += 26;
    }
    else
    {
        x0 += 0; x1 += 1; y0 += 24; y1 += 26;
    }
#endif

#ifdef MODEL7789
    x0 += 0; x1 += 1; y0 += 24; y1 += 26;
#endif

    YSH = (uint8)(y0 >> 8);
    YSL = (uint8)y0;

    YEH = (uint8)(y1 >> 8);
    YEL = (uint8)y1;

#define CMD_CASET 0x2A      // Column Address Set
#define CMD_RASET 0x2B      // Row Address Set
#define CMD_RAMWR 0x2C      // Memory Write

    Write_Cmd(CMD_CASET);
    Write_Data(0x00);
    Write_Data(x0);
    Write_Data(0x00);
    Write_Data(x1);
    Write_Cmd(CMD_RASET);
    Write_Data(YSH);
    Write_Data(YSL);
    Write_Data(YEH);
    Write_Data(YEL);
    Write_Cmd(CMD_RAMWR);
}


void ST7735_89::WriteBuffer(int y0)
{
#ifdef MODEL7735
    LCD_SetPos_Horizontal(0, Display::WIDTH - 1, (uint)y0, (uint)(y0 + Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT - 1));

    pinDC_RS.ToHi();

    for (int y = 0; y < Display::HEIGHT / Display::NUMBER_PARTS_HEIGHT; y++)
    {
        uint8 *points = Display::Buffer::GetLine(y);

        for (int i = 0; i < Display::WIDTH; i++)
        {
            uint16 word = Color::colors[*points++];

            SPI_DATA(SPI0) = (uint)(word >> 8);
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            SPI_DATA(SPI0) = (uint)((uint8)word);
        }
    }
#endif


#ifdef MODEL7789
    (void)y0;

    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 1000)
    {
        return;
    }

    meter.Reset();

    Write_Cmd(0x2a);     //Column address set
    Write_Data(0x00);    //start column
    Write_Data(0x02);
    Write_Data(0x00);    //end column
    Write_Data(0xEE);

    Write_Cmd(0x2b);     //Row address set
    Write_Data(0x00);    //start row
    Write_Data(0x02);
    Write_Data(0x01);    //end row
    Write_Data(0x3E);
    Write_Cmd(0x2C);     //Memory write

    pinDC_RS.ToHi();

    static uint16 word = 1;
    word += 1023;

    for (int ROW = 0; ROW < 240; ROW++)             //ROW loop
    {
        for (int column = 0; column < 320; column++)  //column loop
        {
            SPI_DATA(SPI0) = (uint)(word >> 8);
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            __asm("nop");
            SPI_DATA(SPI0) = (uint8)(word);
        }
    }
#endif
}
