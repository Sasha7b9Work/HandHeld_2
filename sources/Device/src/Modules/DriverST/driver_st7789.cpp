// 2026/01/31 11:19:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


#define set_0   0x01
#define set_1   0x02
#define set_2   0x04
#define set_3   0x08    
#define set_4   0x10
#define set_5   0x20
#define set_6   0x40
#define set_7   0x80

#define clr_0   0xFE
#define clr_1   0xFD
#define clr_2   0xFB
#define clr_3   0xF7    
#define clr_4   0xEF
#define clr_5   0xDF
#define clr_6   0xBF
#define clr_7   0x7F


PinOut pinDC_RS(GPIOB, GPIO_PIN_11);    // PB11
PinOut pinRES(GPIOB, GPIO_PIN_10);      // PB10
PinOut pinBKG(GPIOA, GPIO_PIN_4);       // PA4
PinOut pinON(GPIOB, GPIO_PIN_2);        // PB2
PinOut pinSCK(GPIOA, GPIO_PIN_5);       // PA5
PinOut pinSDA(GPIOA, GPIO_PIN_7);       // PA7


#define SPI_SCK_0   pinSCK.ToLow()

#define SPI_SCK_1   pinSCK.ToHi()

#define SPI_SDA_0   pinSDA.ToLow()

#define SPI_SDA_1   pinSDA.ToHi()

#define SPI_RST_0   pinRES.ToLow()

#define SPI_RST_1   pinRES.ToHi()

#define SPI_DC_0    pinDC_RS.ToLow()

#define SPI_DC_1    pinDC_RS.ToHi()

#define SPI_CS_0

#define SPI_CS_1

//#define BL_0

#define BL_1        pinBKG.ToHi()

#define     RED          0XF800	  //红色
#define     GREEN        0X07E0	  //绿色
#define     BLUE         0X001F	  //蓝色
#define     WHITE        0XFFFF	  //白色

#define TFT_COLUMN_NUMBER 240
#define TFT_LINE_NUMBER 320
#define TFT_COLUMN_OFFSET 0

//#define PIC_NUM 28800			//图片数据大小


//static const unsigned char  *point;
unsigned char  chines_word[][32] =   //汉字码
{
    {0x00,0x00,0xE4,0x3F,0x28,0x20,0x28,0x25,0x81,0x08,0x42,0x10,0x02,0x02,0x08,0x02},
    {0xE8,0x3F,0x04,0x02,0x07,0x07,0x84,0x0A,0x44,0x12,0x34,0x62,0x04,0x02,0x00,0x02},/*"深",0*/

    {0x88,0x20,0x88,0x24,0x88,0x24,0x88,0x24,0x88,0x24,0xBF,0x24,0x88,0x24,0x88,0x24},
    {0x88,0x24,0x88,0x24,0x88,0x24,0xB8,0x24,0x87,0x24,0x42,0x24,0x40,0x20,0x20,0x20},/*"圳",1*/

{0x80,0x00,0x80,0x00,0x40,0x01,0x20,0x02,0x10,0x04,0x08,0x08,0xF4,0x17,0x83,0x60},
{0x80,0x00,0xFC,0x1F,0x80,0x00,0x88,0x08,0x90,0x08,0x90,0x04,0xFF,0x7F,0x00,0x00},/*"金",2*/

{0x80,0x00,0x82,0x00,0x84,0x0F,0x44,0x08,0x20,0x04,0xF0,0x3F,0x27,0x22,0x24,0x22},
{0xE4,0x3F,0x04,0x05,0x84,0x0C,0x84,0x54,0x44,0x44,0x24,0x78,0x0A,0x00,0xF1,0x7F},/*"逸",3*/

{0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x00,0x00,0xFC,0x3F,0x04,0x00},
{0xF4,0x1F,0x04,0x00,0xFC,0x7F,0x94,0x10,0x14,0x09,0x12,0x06,0x52,0x18,0x31,0x60},/*"晨",4*/

{0x80,0x00,0x80,0x00,0x80,0x00,0xFC,0x1F,0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F},
{0x84,0x10,0x84,0x10,0x84,0x10,0xFC,0x1F,0x84,0x50,0x80,0x40,0x80,0x40,0x00,0x7F},/*"电",5*/

{0x00,0x00,0xFE,0x1F,0x00,0x08,0x00,0x04,0x00,0x02,0x80,0x01,0x80,0x00,0xFF,0x7F},
{0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xA0,0x00,0x40,0x00}/*"子",6*/
};


void delay_us(unsigned int _us_time)
{
    _us_time *= 100;
    unsigned char x = 0;
    for (; _us_time > 0; _us_time--)
    {
        x++;
    }
}
void Delay_ms(unsigned int _ms_time)
{
    _ms_time *= 30;
    unsigned int i, j;
    for (i = 0; i < _ms_time; i++)
    {
        for (j = 0; j < 900; j++)
        {
        }
    }
}


void SPI_SendByte(unsigned  char byte)
{
    unsigned char counter;

    for (counter = 0; counter < 8; counter++)
    {
        SPI_SCK_0;
        if ((byte & 0x80) == 0)
        {
            SPI_SDA_0;
        }
        else SPI_SDA_1;
        byte = (unsigned char)(byte << 1);
        SPI_SCK_1;
    }
    SPI_SCK_0;
}

void TFT_SEND_CMD(unsigned char o_command)
{
    SPI_DC_0;
    SPI_CS_0;

    SPI_SendByte(o_command);
    SPI_CS_1;
}

void TFT_SEND_DATA(unsigned  char o_data)
{
    SPI_DC_1;
    SPI_CS_0;
    SPI_SendByte(o_data);
    SPI_CS_1;
}

void TFT_clear(void)
{
    unsigned int ROW, column;
    TFT_SEND_CMD(0x2a);     //Column address set
    TFT_SEND_DATA(0x00);    //start column
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end column
    TFT_SEND_DATA(0xF0);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(0x00);    //start row
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x01);    //end row
    TFT_SEND_DATA(0x40);
    TFT_SEND_CMD(0x2C);     //Memory write
    for (ROW = 0; ROW < TFT_LINE_NUMBER; ROW++)             //ROW loop
    {

        for (column = 0; column < TFT_COLUMN_NUMBER; column++)  //column loop
        {

            TFT_SEND_DATA(0xFF);
            TFT_SEND_DATA(0xFF);
        }
    }
}


void TFT_full(unsigned int color)
{
    unsigned int ROW, column;
    TFT_SEND_CMD(0x2a);     //Column address set
    TFT_SEND_DATA(0x00);    //start column
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end column
    TFT_SEND_DATA(0xF0);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(0x00);    //start row
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x01);    //end row
    TFT_SEND_DATA(0x40);
    TFT_SEND_CMD(0x2C);     //Memory write
    for (ROW = 0; ROW < TFT_LINE_NUMBER; ROW++)             //ROW loop
    {

        for (column = 0; column < TFT_COLUMN_NUMBER; column++) //column loop
        {

            TFT_SEND_DATA((unsigned char)(color >> 8));
            TFT_SEND_DATA((unsigned char)color);
        }
    }
}


void TFT_init(void)     // ST7789V2
{
    {
        SPI_SCK_1;          // 特别注意！！
        SPI_RST_0;
        Delay_ms(50);//1000
        SPI_RST_1;
        Delay_ms(10);//1000

        pinON.ToLow();
        pinBKG.ToHi();

        pinRES.ToLow();
        pinDC_RS.ToLow();

        Delay_ms(50);
        pinRES.ToHi();
        Delay_ms(150);
    }

    /*
    {
        // Adafruit_ST7789s

        TFT_SEND_CMD(0x01);         // RESET
        Delay_ms(150);

        TFT_SEND_CMD(0xF0);         // Unlock manufacturer
        TFT_SEND_DATA(0xC3);

        TFT_SEND_CMD(0xF0);
        TFT_SEND_DATA(0x96);

        TFT_SEND_CMD(0xC5);         // VCOM Control
        TFT_SEND_DATA(0x1C);

        TFT_SEND_CMD(0x36);         // MADCTL
        TFT_SEND_DATA(0x48);

        TFT_SEND_CMD(0x3A);         // COLMOD
        TFT_SEND_DATA(0x55);
        Delay_ms(10);

        TFT_SEND_CMD(0xB0);         // Interface control
        TFT_SEND_DATA(0x80);

        TFT_SEND_CMD(0xB4);         // Inversion control
        TFT_SEND_DATA(0x00);

        TFT_SEND_CMD(0xB6);         // Display Function Control
        TFT_SEND_DATA(0x80);
        TFT_SEND_DATA(0x02);
        TFT_SEND_DATA(0x3B);

        TFT_SEND_CMD(0xB7);         // Entry Mode
        TFT_SEND_DATA(0xC6);

        TFT_SEND_CMD(0xF0);         // Lock manufacturer command
        TFT_SEND_DATA(0x69);

        TFT_SEND_CMD(0xF0);
        TFT_SEND_DATA(0x3C);

        TFT_SEND_CMD(0x11);             // SLPOUT : Sleep Out
        Delay_ms(150);

        TFT_SEND_CMD(0x29);             // DISPON
        Delay_ms(10);
    }
    */

    {
        // Adafruit_ST7789

        TFT_SEND_CMD(0x01);             // RESET
        Delay_ms(150);

        TFT_SEND_CMD(0x11);             // SLPOUT : Sleep Out
        Delay_ms(150);

        TFT_SEND_CMD(0x3A);             // COLMOD
        TFT_SEND_DATA(0x55);
        Delay_ms(10);

        TFT_SEND_CMD(0x36);             // MADCTL
        TFT_SEND_DATA(0x08);

        TFT_SEND_CMD(0x2A);             // CASET
        TFT_SEND_DATA(0x00);
        TFT_SEND_DATA(0x00);
        TFT_SEND_DATA(0x00);
        TFT_SEND_DATA(240);

        TFT_SEND_CMD(0x2B);             // RASET
        TFT_SEND_DATA(0x00);
        TFT_SEND_DATA(0x00);
        TFT_SEND_DATA(320 >> 8);
        TFT_SEND_DATA(320 & 0xFF);

        TFT_SEND_CMD(0x21);             // INVON
        Delay_ms(10);

        TFT_SEND_CMD(0x13);             // NORON
        Delay_ms(10);

        TFT_SEND_CMD(0x29);             // DISPON
        Delay_ms(150);
    }
}


void display_char16_16(unsigned int x, unsigned int y, unsigned long color, unsigned char word_serial_number)
{
    unsigned int column;
    unsigned char tm = 0, temp = 0, xxx = 0;

    TFT_SEND_CMD(0x2a);    //Column address set
    TFT_SEND_DATA((unsigned char)(x >> 8));    //start column
    TFT_SEND_DATA((unsigned char)x);
    x = x + 15;
    TFT_SEND_DATA((unsigned char)(x >> 8));    //end column
    TFT_SEND_DATA((unsigned char)x);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA((unsigned char)(y >> 8));    //start row
    TFT_SEND_DATA((unsigned char)y);
    y = y + 15;
    TFT_SEND_DATA((unsigned char)(y >> 8));    //end row
    TFT_SEND_DATA((unsigned char)y);
    TFT_SEND_CMD(0x2C);     //Memory write


    for (column = 0; column < 32; column++)  //column loop
    {
        temp = chines_word[word_serial_number][xxx];
        for (tm = 0; tm < 8; tm++)
        {
            if (temp & 0x01)
            {
                TFT_SEND_DATA((unsigned char)(color >> 8));
                TFT_SEND_DATA((unsigned char)color);
            }
            else
            {
                TFT_SEND_DATA(0XFF);
                TFT_SEND_DATA(0XFF);
            }
            temp >>= 1;
        }
        xxx++;

    }
}

void IO_init(void)
{
    pinDC_RS.Init();
    pinRES.Init();
    pinBKG.Init();
    pinON.Init();
    pinSCK.Init();
    pinSDA.Init();
}

void ST7789_Init()
{
    IO_init();
    SPI_SCK_0;
    while (true)
    {
        TFT_init();
    }
    BL_1;

    while (1)
    {

        TFT_full(RED);
        Delay_ms(10);
        TFT_full(GREEN);
        Delay_ms(20);
        TFT_full(BLUE);
        Delay_ms(20);
        TFT_clear();


        display_char16_16(20, 160, BLUE, 0);
        display_char16_16(36, 160, GREEN, 1);
        display_char16_16(60, 160, RED, 2);
        display_char16_16(76, 160, BLUE, 3);
        display_char16_16(92, 160, GREEN, 4);
        display_char16_16(118, 160, BLUE, 5);
        display_char16_16(134, 160, RED, 6);
        Delay_ms(10);
    }
}


void ST7789_WriteBuffer(int16 /*x0*/, int16 /*y0*/, int16 /*x1*/, int16 /*y1*/, uint8 * /*buffer*/)
{

}


void ST7789_WriteCommand(uint8)
{

}
