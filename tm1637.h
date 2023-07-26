/*
 * tm1637.h
 *
 *  Created on: May 15, 2022
 *      Author: Duman
 */

#ifndef TM1637_H_
#define TM1637_H_

#include <def.h>
#include <stdint.h>

#ifndef PIN_SET
#define PIN_SET(PORT, PIN)			PORT->BSRR |= (1 << PIN)
#endif

#ifndef PIN_CLR
#define PIN_CLR(PORT, PIN)			PORT->BRR |= (1 << PIN)
#endif

// Main Settings
#define	TM1637_BRIGHTNESS_MAX		(7)
#define	TM1637_POSITION_MAX			(4)

// TM1637 commands
#define	TM1637_CMD_SET_DATA			0x40
#define	TM1637_CMD_SET_ADDR			0xC0
#define	TM1637_CMD_SET_DSIPLAY		0x80

// TM1637 data settings (use bitwise OR to contruct complete command)
#define	TM1637_SET_DATA_WRITE		0x00 // write data to the display register
#define	TM1637_SET_DATA_READ		0x02 // read the key scan data
#define	TM1637_SET_DATA_A_ADDR		0x00 // automatic address increment
#define	TM1637_SET_DATA_F_ADDR		0x04 // fixed address
#define	TM1637_SET_DATA_M_NORM		0x00 // normal mode
#define	TM1637_SET_DATA_M_TEST		0x10 // test mode

// TM1637 display control command set (use bitwise OR to consruct complete command)
#define	TM1637_SET_DISPLAY_OFF		0x00 // off
#define	TM1637_SET_DISPLAY_ON		0x08 // on

#define TM1637_SCK_LOW		PIN_CLR(SEVEN_SEGMENT_CLK_PORT, SEVEN_SEGMENT_CLK_PIN)
#define TM1637_SCK_HIGH		PIN_SET(SEVEN_SEGMENT_CLK_PORT, SEVEN_SEGMENT_CLK_PIN)

#define TM1637_SDO_LOW		PIN_CLR(SEVEN_SEGMENT_SDA_PORT, SEVEN_SEGMENT_SDA_PIN)
#define TM1637_SDO_HIGH		PIN_SET(SEVEN_SEGMENT_SDA_PORT, SEVEN_SEGMENT_SDA_PIN)

void TM1637_set_brightness(const uint8_t value);
void TM1637_display_segments(const uint8_t position, const uint8_t segments);
void TM1637_display_spec(const uint8_t position, const uint8_t spec);
void TM1637_send_config(const uint8_t enable, const uint8_t brightness);
void TM1637_send_command(const uint8_t value);
void TM1637_clear();

#endif /* TM1637_H_ */
