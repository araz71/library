/******************************************************************************
 SSD1306 OLED driver for CCS PIC C compiler (SSD1306OLED.c)                   *
 Reference: Adafruit Industries SSD1306 OLED driver and graphics library.     *
 *
 The driver is for I2C mode only.                                             *
 *
 https://simple-circuit.com/                                                   *
 *
 *******************************************************************************
 *******************************************************************************
 This is a library for our Monochrome OLEDs based on SSD1306 drivers          *
 *
 Pick one up today in the adafruit shop!                                     *
 ------> http://www.adafruit.com/category/63_98                              *
 *
 Adafruit invests time and resources providing this open source code,         *
 please support Adafruit and open-source hardware by purchasing               *
 products from Adafruit!                                                      *
 *
 Written by Limor Fried/Ladyada  for Adafruit Industries.                     *
 BSD license, check license.txt for more information                          *
 All text above, and the splash screen must be included in any redistribution *
 *******************************************************************************/
#include <ssd1306.h>

bool_enu wrap = TRUE;

const char Font[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00,
		0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24,
		0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62, 0x36, 0x49, 0x56,
		0x20, 0x50, 0x00, 0x08, 0x07, 0x03, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x00,
		0x00, 0x41, 0x22, 0x1C, 0x00, 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x08, 0x08,
		0x3E, 0x08, 0x08, 0x00, 0x80, 0x70, 0x30, 0x00, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x00, 0x00, 0x60, 0x60, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3E,
		0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x72, 0x49, 0x49,
		0x49, 0x46, 0x21, 0x41, 0x49, 0x4D, 0x33, 0x18, 0x14, 0x12, 0x7F, 0x10,
		0x27, 0x45, 0x45, 0x45, 0x39, 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x41, 0x21,
		0x11, 0x09, 0x07, 0x36, 0x49, 0x49, 0x49, 0x36, 0x46, 0x49, 0x49, 0x29,
		0x1E, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x40, 0x34, 0x00, 0x00, 0x00,
		0x08, 0x14, 0x22, 0x41, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 0x41, 0x22,
		0x14, 0x08, 0x02, 0x01, 0x59, 0x09, 0x06, 0x3E, 0x41, 0x5D, 0x59, 0x4E,
		0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41,
		0x41, 0x41, 0x22, 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x7F, 0x49, 0x49, 0x49,
		0x41, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x3E, 0x41, 0x41, 0x51, 0x73, 0x7F,
		0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x20, 0x40, 0x41,
		0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40, 0x40,
		0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41,
		0x41, 0x41, 0x3E, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51, 0x21,
		0x5E, 0x7F, 0x09, 0x19, 0x29, 0x46 };
const char Font2[] = { 0x26, 0x49, 0x49, 0x49, 0x32, 0x03, 0x01, 0x7F, 0x01,
		0x03, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F,
		0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14, 0x63, 0x03, 0x04, 0x78,
		0x04, 0x03, 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x41,
		0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41, 0x41, 0x7F, 0x04, 0x02,
		0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x03, 0x07, 0x08,
		0x00, 0x20, 0x54, 0x54, 0x78, 0x40, 0x7F, 0x28, 0x44, 0x44, 0x38, 0x38,
		0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x28, 0x7F, 0x38, 0x54, 0x54,
		0x54, 0x18, 0x00, 0x08, 0x7E, 0x09, 0x02, 0x18, 0xA4, 0xA4, 0x9C, 0x78,
		0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x20, 0x40,
		0x40, 0x3D, 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x41, 0x7F, 0x40,
		0x00, 0x7C, 0x04, 0x78, 0x04, 0x78, 0x7C, 0x08, 0x04, 0x04, 0x78, 0x38,
		0x44, 0x44, 0x44, 0x38, 0xFC, 0x18, 0x24, 0x24, 0x18, 0x18, 0x24, 0x24,
		0x18, 0xFC, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x48, 0x54, 0x54, 0x54, 0x24,
		0x04, 0x04, 0x3F, 0x44, 0x24, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x1C, 0x20,
		0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x28, 0x10, 0x28,
		0x44, 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00,
		0x08, 0x36, 0x41, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x41, 0x36,
		0x08, 0x00, 0x02, 0x01, 0x02, 0x04, 0x02 };

static uint8_t ssd1306_buffer[512] ; //shmem_buf;

void ssd1306_clear_buffer(uint16_t size) {
	memset(ssd1306_buffer, 0, size);
}

void ssd1306_command(uint8_t c) {
	uint8_t control = 0x00;   // Co = 0, D/C = 0
	soft_i2c_start(I2C_FOR_LCD);
	soft_i2c_write(I2C_FOR_LCD, _i2caddr);
	soft_i2c_write(I2C_FOR_LCD, control);
	soft_i2c_write(I2C_FOR_LCD, c);
	soft_i2c_stop(I2C_FOR_LCD);
}

void SSD1306_Init() {
	uint8_t vccstate = SSD1306_SWITCHCAPVCC;
	_vccstate = vccstate;
	_i2caddr = SSD1306_I2C_ADDRESS;

	SSD1306_ClearDisplay();

	// Init sequence
	ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	ssd1306_command(0x80);                           // the suggested ratio 0x80

	ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
	ssd1306_command(SSD1306_LCDHEIGHT - 1);

	ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	ssd1306_command(0x0);                                   // no offset
	ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
	if (vccstate == SSD1306_EXTERNALVCC) {
		ssd1306_command(0x10);
	} else {
		ssd1306_command(0x14);
	}
	ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
	ssd1306_command(0x00);                                // 0x0 act like ks0108
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
	ssd1306_command(SSD1306_COMSCANDEC);

#if defined SSD1306_128_32
	ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_command(0x02);
	ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	ssd1306_command(0x8F);

#elif defined SSD1306_128_64
	ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_command(0x12);
	ssd1306_command(SSD1306_SETCONTRAST);// 0x81
	if (vccstate == SSD1306_EXTERNALVCC)
	{	ssd1306_command(0x9F);}
	else
	{	ssd1306_command(0xCF);}

#elif defined SSD1306_96_16
	ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_command(0x2);//ada x12
	ssd1306_command(SSD1306_SETCONTRAST);// 0x81
	if (vccstate == SSD1306_EXTERNALVCC)
	{	ssd1306_command(0x10);}
	else
	{	ssd1306_command(0xAF);}

#endif

	ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
	if (vccstate == SSD1306_EXTERNALVCC) {
		ssd1306_command(0x22);
	} else {
		ssd1306_command(0xF1);
	}
	ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
	ssd1306_command(0x40);
	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

	ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

	ssd1306_command(SSD1306_DISPLAYON);                 //--turn on oled panel

	// set cursor to (0, 0)
	x_pos = 0;
	y_pos = 0;
	// set text size to 1
	text_size = 1;
}

void SSD1306_DrawPixel(uint8_t x, uint8_t y, bool_enu color) {
	if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
		return;

	if (color)
		ssd1306_buffer[x + (uint16_t) (y / 8) * SSD1306_LCDWIDTH] |= (1
				<< (y & 7));
	else
		ssd1306_buffer[x + (uint16_t) (y / 8) * SSD1306_LCDWIDTH] &= ~(1
				<< (y & 7));
}

void SSD1306_StartScrollRight(uint8_t start, uint8_t stop) {
	ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollLeft(uint8_t start, uint8_t stop) {
	ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X00);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollDiagRight(uint8_t start, uint8_t stop) {
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StartScrollDiagLeft(uint8_t start, uint8_t stop) {
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_StopScroll(void) {
	ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void SSD1306_Dim(bool_enu dim) {
	uint8_t contrast;
	if (dim)
		contrast = 0; // Dimmed display
	else {
		if (_vccstate == SSD1306_EXTERNALVCC)
			contrast = 0x9F;
		else
			contrast = 0xCF;
	}
	// the range of contrast to too small to be really useful
	// it is useful to dim the display
	ssd1306_command(SSD1306_SETCONTRAST);
	ssd1306_command(contrast);
}

void SSD1306_Display(void) {
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0);   // Column start address (0 = reset)
	ssd1306_command(SSD1306_LCDWIDTH - 1); // Column end address (127 = reset)

	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0); // Page start address (0 = reset)
#if SSD1306_LCDHEIGHT == 64
	ssd1306_command(7); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 32
	ssd1306_command(3); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 16
	ssd1306_command(1); // Page end address
#endif

	for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++) {
		// send a bunch of data in one xmission
		soft_i2c_start(I2C_FOR_LCD);
		soft_i2c_write(I2C_FOR_LCD, _i2caddr);
		soft_i2c_write(I2C_FOR_LCD, 0x40);
		for (uint8_t x = 0; x < 16; x++) {
			soft_i2c_write(I2C_FOR_LCD, ssd1306_buffer[i]);
			i++;
		}
		i--;
		soft_i2c_stop(I2C_FOR_LCD);
	}
}

void SSD1306_ClearDisplay(void) {
	for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++)
		ssd1306_buffer[i] = 0;
}

void SSD1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
	bool_enu steep;
	int8_t ystep;
	uint8_t dx, dy;
	int16_t err;
	steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		ssd1306_swap(x0, y0);
		ssd1306_swap(x1, y1);
	}
	if (x0 > x1) {
		ssd1306_swap(x0, x1);
		ssd1306_swap(y0, y1);
	}
	dx = x1 - x0;
	dy = abs(y1 - y0);

	err = dx / 2;
	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	for (; x0 <= x1; x0++) {
		if (steep) {
			SSD1306_DrawPixel(y0, x0, 1);
		} else {
			SSD1306_DrawPixel(x0, y0, 1);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void SSD1306_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w) {
	SSD1306_DrawLine(x, y, x + w - 1, y);
}

void SSD1306_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h) {
	SSD1306_DrawLine(x, y, x, y + h - 1);
}

void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	for (int16_t i = x; i < x + w; i++)
		SSD1306_DrawFastVLine(i, y, h);
}

void SSD1306_FillScreen() {
	SSD1306_FillRect(0, 0, SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT);
}

// Draw a rectangle
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	SSD1306_DrawFastHLine(x, y, w);
	SSD1306_DrawFastHLine(x, y + h - 1, w);
	SSD1306_DrawFastVLine(x, y, h);
	SSD1306_DrawFastVLine(x + w - 1, y, h);
}

// invert the display
void SSD1306_InvertDisplay(bool_enu i) {
	if (i)
		ssd1306_command(SSD1306_INVERTDISPLAY_);
	else
		ssd1306_command(SSD1306_NORMALDISPLAY);
}

void SSD1306_SetTextWrap(bool_enu w) {
	wrap = w;
}

void SSD1306_DrawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t size) {
	SSD1306_GotoXY(x, y);
	SSD1306_TextSize(size);
	SSD1306_Print(c);
}

void SSD1306_DrawText(uint8_t x, uint8_t y, const char *_text, uint8_t size) {
	SSD1306_GotoXY(x, y);
	SSD1306_TextSize(size);
	while (*_text != '\0')
		SSD1306_Print(*_text++);

}

// move cursor to position (x, y)
void SSD1306_GotoXY(uint8_t x, uint8_t y) {
	if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
		return;
	x_pos = x;
	y_pos = y;
}

// set text size
void SSD1306_TextSize(uint8_t t_size) {
	if (t_size < 1)
		t_size = 1;
	text_size = t_size;
}

/* print single char
 \a  Set cursor position to upper left (0, 0)
 \b  Move back one position
 \n  Go to start of current line
 \r  Go to line below
 */
void SSD1306_Print(uint8_t c) {
	bool_enu _color;
	uint8_t i, j, line;

	if (c == ' ' && x_pos == 0 && wrap)
		return;
	if (c == '\a') {
		x_pos = y_pos = 0;
		return;
	}
	if ((c == '\b') && (x_pos >= text_size * 6)) {
		x_pos -= text_size * 6;
		return;
	}
	if (c == '\r') {
		x_pos = 0;
		return;
	}
	if (c == '\n') {
		y_pos += text_size * 8;
		if ((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
			y_pos = 0;
		return;
	}

	if ((c < ' ') || (c > '~'))
		c = '?';

	for (i = 0; i < 5; i++) {
		if (c < 'S')
			line = Font[(c - ' ') * 5 + i];
		else
			line = Font2[(c - 'S') * 5 + i];

		for (j = 0; j < 7; j++, line >>= 1) {
			if (line & 0x01)
				_color = TRUE;
			else
				_color = FALSE;
			if (text_size == 1)
				SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
			else
				SSD1306_FillRect(x_pos + (i * text_size),
						y_pos + (j * text_size), text_size, text_size);
		}
	}

	x_pos += text_size * 6;

	if (x_pos > (SSD1306_LCDWIDTH + text_size * 6))
		x_pos = SSD1306_LCDWIDTH;

	if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH) {
		x_pos = 0;
		y_pos += text_size * 8;
		if ((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
			y_pos = 0;
	}
}

// print custom char (dimension: 7x5 pixel)
void SSD1306_PutCustomC(const uint8_t *c) {
	bool_enu _color;
	uint8_t i, j, line;

	for (i = 0; i < 5; i++) {
		line = c[i];

		for (j = 0; j < 7; j++, line >>= 1) {
			if (line & 0x01)
				_color = TRUE;
			else
				_color = FALSE;
			if (text_size == 1)
				SSD1306_DrawPixel(x_pos + i, y_pos + j, _color);
			else
				SSD1306_FillRect(x_pos + (i * text_size),
						y_pos + (j * text_size), text_size, text_size);
		}
	}

	x_pos += (text_size * 6);

	if (x_pos > (SSD1306_LCDWIDTH + text_size * 6))
		x_pos = SSD1306_LCDWIDTH;

	if (wrap && (x_pos + (text_size * 5)) > SSD1306_LCDWIDTH) {
		x_pos = 0;
		y_pos += text_size * 8;
		if ((y_pos + text_size * 7) > SSD1306_LCDHEIGHT)
			y_pos = 0;
	}
}

// draw BMP stored in ROM
void SSD1306_ROMBMP(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h) {
	for (uint16_t i = 0; i < h / 8; i++) {
		for (uint16_t j = 0; j < (uint16_t) w * 8; j++) {
			if (bit_test(bitmap[j / 8 + i * w], j % 8) == 1)
				SSD1306_DrawPixel(x + j / 8, y + i * 8 + (j % 8), 1);
			else
				SSD1306_DrawPixel(x + j / 8, y + i * 8 + (j % 8), 0);
		}
	}
}

void SSD1306_SpecialBMP(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h) {
	for (uint16_t i = 0; i < h; i++) {
		for (uint16_t j = 0; j < w; j++) {
			if (bitmap[j] & (1<< (i % 8)))
				SSD1306_DrawPixel(j + x, i + y, 1);
			else
				SSD1306_DrawPixel(j + x, i + y, 0);
		}
	}
}
// end of driver code.
