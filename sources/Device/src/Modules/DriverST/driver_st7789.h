#ifndef __ST7789_H
#define __ST7789_H

//#include "fonts.h"
//#include "main.h"

/* choose a Hardware SPI port to use. */
#define ST7789_SPI_PORT hspi1

//extern SPI_HandleTypeDef ST7789_SPI_PORT;

/* If u need CS control, comment below*/
#define CFG_NO_CS

/* Pin connection*/
#define ST7789_RST_PORT ST7789_RST_GPIO_Port
#define ST7789_RST_PIN  ST7789_RST_Pin
#define ST7789_DC_PORT  ST7789_DC_GPIO_Port
#define ST7789_DC_PIN   ST7789_DC_Pin

#ifndef CFG_NO_CS
#define ST7789_CS_PORT  ST7789_CS_GPIO_Port
#define ST7789_CS_PIN   ST7789_CS_Pin
#endif

/* If u need Backlight control, uncomment below */
//#define BLK_PORT
//#define BLK_PIN


/*
 * Comment one to use another.
 * 3 parameters can be choosed
 * 135x240(0.96 inch) & 240x240(1.3inch) & 170x320(1.9inch)
 * X_SHIFT & Y_SHIFT are used to adapt different display's resolution
 */

 /* Choose a type you are using */
 //#define USING_135X240
#define USING_240X240
//#define USING_170X320

/* Choose a display rotation you want to use: (0-3) */
//#define ST7789_ROTATION 0
//#define ST7789_ROTATION 1
#define ST7789_ROTATION 2				//  use Normally on 240x240
//#define ST7789_ROTATION 3

#ifdef USING_135X240

#if ST7789_ROTATION == 0
#define ST7789_WIDTH 135
#define ST7789_HEIGHT 240
#define X_SHIFT 53
#define Y_SHIFT 40
#endif

#if ST7789_ROTATION == 1
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 135
#define X_SHIFT 40
#define Y_SHIFT 52
#endif

#if ST7789_ROTATION == 2
#define ST7789_WIDTH 135
#define ST7789_HEIGHT 240
#define X_SHIFT 52
#define Y_SHIFT 40
#endif

#if ST7789_ROTATION == 3
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 135
#define X_SHIFT 40
#define Y_SHIFT 53
#endif

#endif

#ifdef USING_240X240

#define ST7789_WIDTH 240
#define ST7789_HEIGHT 240

#if ST7789_ROTATION == 0
#define X_SHIFT 0
#define Y_SHIFT 80
#elif ST7789_ROTATION == 1
#define X_SHIFT 80
#define Y_SHIFT 0
#elif ST7789_ROTATION == 2
#define X_SHIFT 0
#define Y_SHIFT 0
#elif ST7789_ROTATION == 3
#define X_SHIFT 0
#define Y_SHIFT 0
#endif

#endif

#ifdef USING_170X320

#if ST7789_ROTATION == 0
#define ST7789_WIDTH 170
#define ST7789_HEIGHT 320
#define X_SHIFT 35
#define Y_SHIFT 0
#endif

#if ST7789_ROTATION == 1
#define ST7789_WIDTH 320
#define ST7789_HEIGHT 170
#define X_SHIFT 0
#define Y_SHIFT 35
#endif

#if ST7789_ROTATION == 2
#define ST7789_WIDTH 170
#define ST7789_HEIGHT 320
#define X_SHIFT 35
#define Y_SHIFT 0
#endif

#if ST7789_ROTATION == 3
#define ST7789_WIDTH 320
#define ST7789_HEIGHT 170
#define X_SHIFT 0
#define Y_SHIFT 35
#endif

#endif

/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */

#define __WHITE       0xFFFF
#define __BLACK       0x0000
#define __BLUE        0x001F
#define __RED         0xF800
#define __MAGENTA     0xF81F
#define __GREEN       0x07E0
#define __CYAN        0x7FFF
#define __YELLOW      0xFFE0
#define __GRAY        0X8430
#define __BRED        0XF81F
#define __GRED        0XFFE0
#define __GBLUE       0X07FF
#define __BROWN       0XBC40
#define __BRRED       0XFC07
#define __DARKBLUE    0X01CF
#define __LIGHTBLUE   0X7D7C
#define __GRAYBLUE    0X5458

#define __LIGHTGREEN  0X841F
#define __LGRAY       0XC618
#define __LGRAYBLUE   0XA651
#define __LBBLUE      0X2B12

 /* Control Registers and constant codes */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

/**
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

 /* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD

/* Advanced options */
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

#ifndef CFG_NO_CS
    #define ST7789_Select() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)

    #define ST7789_UnSelect() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)

#else
    #define ST7789_Select()
    #define ST7789_UnSelect()
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

/* Basic functions. */
void ST7789_Init(void);
void ST7789_SetRotation(uint8 m);
void ST7789_Fill_Color(uint16 color);
void ST7789_DrawPixel(int16 x, int16 y, uint16 color);
void ST7789_Fill(int16 xSta, int16 ySta, int16 xEnd, int16 yEnd, uint16 color);
void ST7789_WriteBuffer(int16 x0, int16 y0, int16 x1, int16 y1, uint8 *buffer);
void ST7789_DrawPixel_4px(uint16 x, uint16 y, uint16 color);

/* Graphical functions. */
void ST7789_DrawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color);
void ST7789_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color);
void ST7789_DrawCircle(int16 x0, int16 y0, uint8 r, uint16 color);
void ST7789_DrawImage(uint16 x, uint16 y, uint16 w, uint16 h, const uint16 *data);
void ST7789_InvertColors(uint8 invert);

/* Text functions. */
//void ST7789_WriteChar(uint16 x, uint16 y, char ch, FontDef font, uint16 color, uint16 bgcolor);
//void ST7789_WriteString(uint16 x, uint16 y, const char *str, FontDef font, uint16 color, uint16 bgcolor);

/* Extented Graphical functions. */
void ST7789_DrawFilledRectangle(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color);
void ST7789_DrawTriangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 x3, uint16 y3, uint16 color);
void ST7789_DrawFilledTriangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 x3, uint16 y3, uint16 color);
void ST7789_DrawFilledCircle(int16 x0, int16 y0, int16 r, uint16 color);

/* Command functions */
void ST7789_TearEffect(uint8 tear);

/* Simple test function. */
void ST7789_Test(void);

void ST7789_WriteCommand(uint8);

#ifndef ST7789_ROTATION
#error You should at least choose a display rotation!
#endif

#endif
