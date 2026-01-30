#include "defines.h"
#include "Modules/DriverST/driver_st7789.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "system.h"


#ifdef WIN32
    #pragma warning(disable:4365 4838)
#else
    #if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
        #pragma clang diagnostic ignored "-Wsign-conversion"
        #pragma clang diagnostic ignored "-Wcast-qual"
        #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
        #pragma clang diagnostic ignored "-Wc++11-narrowing"
    #endif
#endif


PinOut pinDC_RS(GPIOB, GPIO_PIN_11);   // PB11
PinOut pinRES(GPIOB, GPIO_PIN_10);     // PB10
PinOut pinBKG(GPIOA, GPIO_PIN_4);      // PA4
PinOut pinON(GPIOB, GPIO_PIN_2);       // PB2

#define HAL_Delay(x)


/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */
void ST7789_WriteCommand(uint8 cmd)
{
	ST7789_Select();
	pinDC_RS.ToLow();
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) { }
	spi_i2s_data_transmit(SPI0, cmd);
	ST7789_UnSelect();
}

/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
static void ST7789_WriteData(uint8 *buff, uint buff_size)
{
	ST7789_Select();
	pinDC_RS.ToHi();

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0)
	{
		uint16 chunk_size = buff_size > 65535U ? 65535U : (uint16)buff_size;
//		HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		for (uint i = 0; i < chunk_size; i++)
		{
			while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE)) { }
			spi_i2s_data_transmit(SPI0, *buff++);
		}
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	ST7789_UnSelect();
}
/**
 * @brief Write data to ST7789 controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */
static void ST7789_WriteSmallData(uint8 data)
{
	ST7789_Select();
	pinDC_RS.ToHi();
//	HAL_SPI_Transmit(&ST7789_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE))
    {
    }
    spi_i2s_data_transmit(SPI0, data);
	ST7789_UnSelect();
}

/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in st7789.h)
 * @return none
 */
void ST7789_SetRotation(uint8 m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m)
	{
	case 0:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
		break;
	case 3:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
static void ST7789_SetAddressWindow(uint16 x0, uint16 y0, uint16 x1, uint16 y1)
{
	ST7789_Select();
	uint16 x_start = (uint16)(x0 + X_SHIFT), x_end = (uint16)(x1 + X_SHIFT);
	uint16 y_start = (uint16)(y0 + Y_SHIFT), y_end = (uint16)(y1 + Y_SHIFT);

	/* Column Address set */
	ST7789_WriteCommand(ST7789_CASET);
	{
		uint8 data[] = { (uint8)(x_start >> 8), (uint8)(x_start & 0xFF), (uint8)(x_end >> 8), (uint8)(x_end & 0xFF) };
		ST7789_WriteData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8 data[] = { y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF };
		ST7789_WriteData(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
	ST7789_UnSelect();
}

/**
 * @brief Initialize ST7789 controller
 * @param none
 * @return none
 */
void ST7789_Init(void)
{
    {
        // PA5 - TFT_SCL SPI0
        // PA7 - TFT_SDA SPI0
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
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

	pinDC_RS.Init();
	pinRES.Init();
	pinBKG.Init();
	pinON.Init();

	pinON.ToLow();
	pinBKG.ToHi();

	HAL_Delay(10);
	pinRES.ToLow();
	HAL_Delay(10);
	pinRES.ToHi();
	HAL_Delay(20);

	ST7789_WriteCommand(ST7789_COLMOD);		//	Set color mode
	ST7789_WriteSmallData(ST7789_COLOR_MODE_16bit);
	ST7789_WriteCommand(0xB2);				//	Porch control
	{
		uint8 data[] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
		ST7789_WriteData(data, sizeof(data));
	}
	ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)

	/* Internal LCD Voltage generator settings */
	ST7789_WriteCommand(0XB7);				//	Gate Control
	ST7789_WriteSmallData(0x35);			//	Default value
	ST7789_WriteCommand(0xBB);				//	VCOM setting
	ST7789_WriteSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
	ST7789_WriteCommand(0xC0);				//	LCMCTRL	
	ST7789_WriteSmallData(0x2C);			//	Default value
	ST7789_WriteCommand(0xC2);				//	VDV and VRH command Enable
	ST7789_WriteSmallData(0x01);			//	Default value
	ST7789_WriteCommand(0xC3);				//	VRH set
	ST7789_WriteSmallData(0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
	ST7789_WriteCommand(0xC4);				//	VDV set
	ST7789_WriteSmallData(0x20);			//	Default value
	ST7789_WriteCommand(0xC6);				//	Frame rate control in normal mode
	ST7789_WriteSmallData(0x0F);			//	Default value (60HZ)
	ST7789_WriteCommand(0xD0);				//	Power control
	ST7789_WriteSmallData(0xA4);			//	Default value
	ST7789_WriteSmallData(0xA1);			//	Default value
	/**************** Division line ****************/

	ST7789_WriteCommand(0xE0);
	{
		uint8 data[] = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23 };
		ST7789_WriteData(data, sizeof(data));
	}

	ST7789_WriteCommand(0xE1);
	{
		uint8 data[] = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23 };
		ST7789_WriteData(data, sizeof(data));
	}
	ST7789_WriteCommand(ST7789_INVON);		//	Inversion ON
	ST7789_WriteCommand(ST7789_SLPOUT);	//	Out of sleep mode
	ST7789_WriteCommand(ST7789_NORON);		//	Normal Display on
	ST7789_WriteCommand(ST7789_DISPON);	//	Main screen turned on	

	HAL_Delay(50);
	ST7789_Fill_Color(__BLACK);				//	Fill with Black.
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill_Color(uint16 color)
{
	uint16 i;
	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	ST7789_Select();

	uint16 j;
	for (i = 0; i < ST7789_WIDTH; i++)
		for (j = 0; j < ST7789_HEIGHT; j++)
		{
			uint8 data[] = { color >> 8, color & 0xFF };
			ST7789_WriteData(data, sizeof(data));
		}
	ST7789_UnSelect();
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel(int16 x, int16 y, uint16 color)
{
	if ((x < 0) || (x >= ST7789_WIDTH) ||
		(y < 0) || (y >= ST7789_HEIGHT))	return;

	ST7789_SetAddressWindow(x, y, x, y);
	uint8 data[] = { color >> 8, color & 0xFF };
	ST7789_Select();
	ST7789_WriteData(data, sizeof(data));
	ST7789_UnSelect();
}

/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill(int16 xSta, int16 ySta, int16 xEnd, int16 yEnd, uint16 color)
{
	if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
		(yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
	ST7789_Select();
	uint16 i, j;
	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++)
		{
			uint8 data[] = { color >> 8, color & 0xFF };
			ST7789_WriteData(data, sizeof(data));
		}
	ST7789_UnSelect();
}


void ST7789_WriteBuffer(int16 x0, int16 y0, int16 x1, int16 y1, uint8 *buffer)
{
	ST7789_SetAddressWindow(x0, y0, x1, y1);

	uint size = x1 - x0;

	for (int i = y0; i <= y1; i++)
	{
		ST7789_WriteData(buffer, size);
		buffer += size;
	}
}


/**
 * @brief Draw a big Pixel at a point
 * @param x&y -> coordinate of the point
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel_4px(uint16 x, uint16 y, uint16 color)
{
	if ((x <= 0) || (x > ST7789_WIDTH) ||
		(y <= 0) || (y > ST7789_HEIGHT))	return;
	ST7789_Select();
	ST7789_Fill(x - 1, y - 1, x + 1, y + 1, color);
	ST7789_UnSelect();
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789_DrawLine(uint16 x0, uint16 y0, uint16 x1, uint16 y1,
	uint16 color)
{
	uint16 swap;
	uint16 steep = ABS(y1 - y0) > ABS(x1 - x0);
	if (steep)
	{
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
		//_swap_int16_t(x0, y0);
		//_swap_int16_t(x1, y1);
	}

	if (x0 > x1)
	{
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
		//_swap_int16_t(x0, x1);
		//_swap_int16_t(y0, y1);
	}

	int16 dx, dy;
	dx = x1 - x0;
	dy = ABS(y1 - y0);

	int16 err = dx / 2;
	int16 ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			ST7789_DrawPixel(y0, x0, color);
		}
		else
		{
			ST7789_DrawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

/**
 * @brief Draw a Rectangle with single color
 * @param xi&yi -> 2 coordinates of 2 top points.
 * @param color -> color of the Rectangle line
 * @return none
 */
void ST7789_DrawRectangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 color)
{
	ST7789_Select();
	ST7789_DrawLine(x1, y1, x2, y1, color);
	ST7789_DrawLine(x1, y1, x1, y2, color);
	ST7789_DrawLine(x1, y2, x2, y2, color);
	ST7789_DrawLine(x2, y1, x2, y2, color);
	ST7789_UnSelect();
}

/**
 * @brief Draw a circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle line
 * @return  none
 */
void ST7789_DrawCircle(int16 x0, int16 y0, uint8 r, uint16 color)
{
	int16 f = 1 - r;
	int16 ddF_x = 1;
	int16 ddF_y = -2 * r;
	int16 x = 0;
	int16 y = r;

	ST7789_Select();
	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(x0 + x, y0 + y, color);
		ST7789_DrawPixel(x0 - x, y0 + y, color);
		ST7789_DrawPixel(x0 + x, y0 - y, color);
		ST7789_DrawPixel(x0 - x, y0 - y, color);

		ST7789_DrawPixel(x0 + y, y0 + x, color);
		ST7789_DrawPixel(x0 - y, y0 + x, color);
		ST7789_DrawPixel(x0 + y, y0 - x, color);
		ST7789_DrawPixel(x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}

/**
 * @brief Draw an Image on the screen
 * @param x&y -> start point of the Image
 * @param w&h -> width & height of the Image to Draw
 * @param data -> pointer of the Image array
 * @return none
 */
void ST7789_DrawImage(uint16 x, uint16 y, uint16 w, uint16 h, const uint16 *data)
{
	if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT))
		return;
	if ((x + w - 1) >= ST7789_WIDTH)
		return;
	if ((y + h - 1) >= ST7789_HEIGHT)
		return;

	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	ST7789_WriteData((uint8 *)data, sizeof(uint16) * w * h);
	ST7789_UnSelect();
}

/**
 * @brief Invert Fullscreen color
 * @param invert -> Whether to invert
 * @return none
 */
void ST7789_InvertColors(uint8 invert)
{
	ST7789_Select();
	ST7789_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
	ST7789_UnSelect();
}

/**
 * @brief Write a char
 * @param  x&y -> cursor of the start point.
 * @param ch -> char to write
 * @param font -> fontstyle of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return  none
 */
/*
void ST7789_WriteChar(uint16 x, uint16 y, char ch, FontDef font, uint16 color, uint16 bgcolor)
{
	uint32_t i, b, j;
	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++)
	{
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++)
		{
			if ((b << j) & 0x8000)
			{
				uint8_t data[] = { color >> 8, color & 0xFF };
				ST7789_WriteData(data, sizeof(data));
			}
			else
			{
				uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
				ST7789_WriteData(data, sizeof(data));
			}
		}
	}
	ST7789_UnSelect();
}
*/

/**
 * @brief Write a string
 * @param  x&y -> cursor of the start point.
 * @param str -> string to write
 * @param font -> fontstyle of the string
 * @param color -> color of the string
 * @param bgcolor -> background color of the string
 * @return  none
 */
/*
void ST7789_WriteString(uint16 x, uint16 y, const char *str, FontDef font, uint16 color, uint16 bgcolor)
{
	ST7789_Select();
	while (*str)
	{
		if (x + font.width >= ST7789_WIDTH)
		{
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT)
			{
				break;
			}

			if (*str == ' ')
			{
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
	ST7789_UnSelect();
}
*/

/**
 * @brief Draw a filled Rectangle with single color
 * @param  x&y -> coordinates of the starting point
 * @param w&h -> width & height of the Rectangle
 * @param color -> color of the Rectangle
 * @return  none
 */
void ST7789_DrawFilledRectangle(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color)
{
	ST7789_Select();
	uint8 i;

	/* Check input parameters */
	if (x >= ST7789_WIDTH ||
		y >= ST7789_HEIGHT)
	{
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= ST7789_WIDTH)
	{
		w = ST7789_WIDTH - x;
	}
	if ((y + h) >= ST7789_HEIGHT)
	{
		h = ST7789_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++)
	{
		/* Draw lines */
		ST7789_DrawLine(x, y + i, x + w, y + i, color);
	}
	ST7789_UnSelect();
}

/**
 * @brief Draw a Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the lines
 * @return  none
 */
void ST7789_DrawTriangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 x3, uint16 y3, uint16 color)
{
	ST7789_Select();
	/* Draw lines */
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x3, y3, color);
	ST7789_DrawLine(x3, y3, x1, y1, color);
	ST7789_UnSelect();
}

/**
 * @brief Draw a filled Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the triangle
 * @return  none
 */
void ST7789_DrawFilledTriangle(uint16 x1, uint16 y1, uint16 x2, uint16 y2, uint16 x3, uint16 y3, uint16 color)
{
	ST7789_Select();
	int16 deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
		yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
		curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1)
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)
	{
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else
	{
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		ST7789_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den)
		{
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
	ST7789_UnSelect();
}

/**
 * @brief Draw a Filled circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle
 * @return  none
 */
void ST7789_DrawFilledCircle(int16 x0, int16 y0, int16 r, uint16 color)
{
	ST7789_Select();
	int16 f = 1 - r;
	int16 ddF_x = 1;
	int16 ddF_y = -2 * r;
	int16 x = 0;
	int16 y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);
	ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}


/**
 * @brief Open/Close tearing effect line
 * @param tear -> Whether to tear
 * @return none
 */
void ST7789_TearEffect(uint8 tear)
{
	ST7789_Select();
	ST7789_WriteCommand(tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */);
	ST7789_UnSelect();
}


/**
 * @brief A Simple test function for ST7789
 * @param  none
 * @return  none
 */
void ST7789_Test(void)
{
	ST7789_Fill_Color(__WHITE);
	HAL_Delay(1000);
//	ST7789_WriteString(10, 20, "Speed Test", Font_11x18, RED, WHITE);
	HAL_Delay(1000);
	ST7789_Fill_Color(__CYAN);
	HAL_Delay(500);
	ST7789_Fill_Color(__RED);
	HAL_Delay(500);
	ST7789_Fill_Color(__BLUE);
	HAL_Delay(500);
	ST7789_Fill_Color(__GREEN);
	HAL_Delay(500);
	ST7789_Fill_Color(__YELLOW);
	HAL_Delay(500);
	ST7789_Fill_Color(__BROWN);
	HAL_Delay(500);
	ST7789_Fill_Color(__DARKBLUE);
	HAL_Delay(500);
	ST7789_Fill_Color(__MAGENTA);
	HAL_Delay(500);
	ST7789_Fill_Color(__LIGHTGREEN);
	HAL_Delay(500);
	ST7789_Fill_Color(__LGRAY);
	HAL_Delay(500);
	ST7789_Fill_Color(__LBBLUE);
	HAL_Delay(500);
	ST7789_Fill_Color(__WHITE);
	HAL_Delay(500);

//	ST7789_WriteString(10, 10, "Font test.", Font_16x26, GBLUE, WHITE);
//	ST7789_WriteString(10, 50, "Hello Steve!", Font_7x10, RED, WHITE);
//	ST7789_WriteString(10, 75, "Hello Steve!", Font_11x18, YELLOW, WHITE);
//	ST7789_WriteString(10, 100, "Hello Steve!", Font_16x26, MAGENTA, WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Rect./Line.", Font_11x18, YELLOW, BLACK);
	ST7789_DrawRectangle(30, 30, 100, 100, __WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Filled Rect.", Font_11x18, YELLOW, BLACK);
	ST7789_DrawFilledRectangle(30, 30, 50, 50, __WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Circle.", Font_11x18, YELLOW, BLACK);
	ST7789_DrawCircle(60, 60, 25, __WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Filled Cir.", Font_11x18, YELLOW, BLACK);
	ST7789_DrawFilledCircle(60, 60, 25, __WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Triangle", Font_11x18, YELLOW, BLACK);
	ST7789_DrawTriangle(30, 30, 30, 70, 60, 40, __WHITE);
	HAL_Delay(1000);

	ST7789_Fill_Color(__RED);
//	ST7789_WriteString(10, 10, "Filled Tri", Font_11x18, YELLOW, BLACK);
	ST7789_DrawFilledTriangle(30, 30, 30, 70, 60, 40, __WHITE);
	HAL_Delay(1000);

	//	If FLASH cannot storage anymore datas, please delete codes below.
	ST7789_Fill_Color(__WHITE);
//	ST7789_DrawImage(0, 0, 128, 128, (uint16 *)saber);
	HAL_Delay(3000);
}
