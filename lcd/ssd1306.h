/**
 * -------------------------------------------------------------------------------------+
 * @brief       SSD1306 OLED Driver
 * -------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @file        ssd1306.h
 * @version     2.0.0
 * @test        AVR Atmega328p
 *
 * @depend      string.h, font.h, twi.h
 * -------------------------------------------------------------------------------------+
 * @brief       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> simplified alphanumeric version for 1 display
 * -------------------------------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stdint.h>
#include <stdlib.h>
#include <soft_i2c.h>
//------------------------------ Definitions ---------------------------------//

#ifndef SSD1306_I2C_ADDRESS
#define SSD1306_I2C_ADDRESS   0x78
#endif

#if !defined SSD1306_128_32 && !defined SSD1306_96_16
#define SSD1306_128_32
#endif
#if defined SSD1306_128_32 && defined SSD1306_96_16
#error "Only one SSD1306 display can be specified at once"
#endif

#if defined SSD1306_128_64
#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT            64
#endif
#if defined SSD1306_128_32
#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT            32
#endif
#if defined SSD1306_96_16
#define SSD1306_LCDWIDTH             96
#define SSD1306_LCDHEIGHT            16
#endif

#define SSD1306_SETCONTRAST          0x81
#define SSD1306_DISPLAYALLON_RESUME  0xA4
#define SSD1306_DISPLAYALLON         0xA5
#define SSD1306_NORMALDISPLAY        0xA6
#define SSD1306_INVERTDISPLAY_       0xA7
#define SSD1306_DISPLAYOFF           0xAE
#define SSD1306_DISPLAYON            0xAF
#define SSD1306_SETDISPLAYOFFSET     0xD3
#define SSD1306_SETCOMPINS           0xDA
#define SSD1306_SETVCOMDETECT        0xDB
#define SSD1306_SETDISPLAYCLOCKDIV   0xD5
#define SSD1306_SETPRECHARGE         0xD9
#define SSD1306_SETMULTIPLEX         0xA8
#define SSD1306_SETLOWCOLUMN         0x00
#define SSD1306_SETHIGHCOLUMN        0x10
#define SSD1306_SETSTARTLINE         0x40
#define SSD1306_MEMORYMODE           0x20
#define SSD1306_COLUMNADDR           0x21
#define SSD1306_PAGEADDR             0x22
#define SSD1306_COMSCANINC           0xC0
#define SSD1306_COMSCANDEC           0xC8
#define SSD1306_SEGREMAP             0xA0
#define SSD1306_CHARGEPUMP           0x8D
#define SSD1306_EXTERNALVCC          0x01
#define SSD1306_SWITCHCAPVCC         0x02

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                      0x2F
#define SSD1306_DEACTIVATE_SCROLL                    0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

uint8_t _i2caddr, _vccstate, x_pos, y_pos, text_size;

void ssd1306_clear_buffer(uint16_t size);

void ssd1306_command(uint8_t c);
void SSD1306_Init();
void SSD1306_DrawPixel(uint8_t x, uint8_t y, bool_enu color);
void SSD1306_StartScrollRight(uint8_t start, uint8_t stop);
void SSD1306_StartScrollLeft(uint8_t start, uint8_t stop);
void SSD1306_StartScrollDiagRight(uint8_t start, uint8_t stop);
void SSD1306_StartScrollDiagLeft(uint8_t start, uint8_t stop);
void SSD1306_StopScroll(void);
void SSD1306_Dim(bool_enu dim);
void SSD1306_Display(void);
void SSD1306_ClearDisplay(void);
void SSD1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void SSD1306_DrawFastHLine(uint8_t x, uint8_t y, uint8_t w);
void SSD1306_DrawFastVLine(uint8_t x, uint8_t y, uint8_t h);
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void SSD1306_FillScreen();
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void SSD1306_DrawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t size);
void SSD1306_DrawText(uint8_t x, uint8_t y, const char *_text, uint8_t size);
void SSD1306_TextSize(uint8_t t_size);
void SSD1306_GotoXY(uint8_t x, uint8_t y);
void SSD1306_Print(uint8_t c);
void SSD1306_PutCustomC(const uint8_t *c);
void SSD1306_SetTextWrap(bool_enu w);
void SSD1306_InvertDisplay(bool_enu i);
void SSD1306_DrawBMP(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w,
		uint8_t h);
void SSD1306_SpecialBMP(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h);

//--------------------------------------------------------------------------//



 #endif
