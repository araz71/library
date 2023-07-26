/*
 * tm1637.c
 *
 *  Created on: May 15, 2022
 *      Author: Duman
 */
#include "tm1637.h"

static uint8_t _config = TM1637_SET_DISPLAY_ON | TM1637_BRIGHTNESS_MAX;
#define TM1637_DELAY_COUNT		0x0F
const uint8_t sevent_segment_codes[] = {
		0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20,		//0
		0x02 | 0x04,									//1
		0x01 | 0x02 | 0x40 | 0x10 | 0x08,				//2
		0x01 | 0x02 | 0x40 | 0x04 | 0x08,				//3
		0x20 | 0x40 | 0x04 | 0x02,						//4
		0x01 | 0x20 | 0x40 | 0x04 | 0x08,				//5
		0x7D,											//6
		0x01 | 0x02 | 0x04,								//7
		0x7F,											//8
		0x6F,											//9
		0x77,											//A
		0x7C,											//b
		0x40 | 0x10 | 0x08,								//c
		0x02 | 0x04 | 0x10 | 0x08 | 0x40,				//d
		0x01 | 0x20 | 0x40 | 0x10 | 0x08,				//e
		0x01 | 0x20 | 0x40 | 0x10						//f
};

static void tm1637_write(uint8_t data) {
	uint8_t i;

	for (i = 0; i < 8; ++i, data >>= 1) {
		TM1637_SCK_LOW;
		for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();

		if (data & 0x01) {
			TM1637_SDO_HIGH;
		} else {
			TM1637_SDO_LOW;
		}
		TM1637_SCK_HIGH;
		for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	}

	TM1637_SCK_HIGH;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	TM1637_SCK_LOW;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();

	TM1637_SCK_HIGH;
	TM1637_SDO_HIGH;
}

static void tm1637_start() {
	TM1637_SDO_HIGH;
	TM1637_SCK_HIGH;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	TM1637_SDO_LOW;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	TM1637_SCK_LOW;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
}

static void tm1637_stop() {
	TM1637_SDO_LOW;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	TM1637_SCK_LOW;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();

	TM1637_SCK_HIGH;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
	TM1637_SDO_HIGH;
	for (int j = 0; j < TM1637_DELAY_COUNT; j++) __NOP();
}

void TM1637_send_command(const uint8_t value) {
	tm1637_start();
	tm1637_write(value);
	tm1637_stop();
}

void TM1637_send_config(const uint8_t enable, const uint8_t brightness) {
	_config = (enable ? TM1637_SET_DISPLAY_ON : TM1637_SET_DISPLAY_OFF)
			| (brightness > TM1637_BRIGHTNESS_MAX ?
					TM1637_BRIGHTNESS_MAX : brightness);

	TM1637_send_command(TM1637_CMD_SET_DSIPLAY | _config);
}

void TM1637_display_segments(const uint8_t position, const uint8_t segments) {
	TM1637_send_command(TM1637_CMD_SET_DATA | TM1637_SET_DATA_F_ADDR);
	tm1637_start();
	tm1637_write(TM1637_CMD_SET_ADDR | (position & (TM1637_POSITION_MAX - 1)));
	tm1637_write(segments);
	tm1637_stop();
}

void TM1637_display_spec(const uint8_t position, const uint8_t spec) {
	TM1637_send_command(TM1637_CMD_SET_DATA | TM1637_SET_DATA_F_ADDR);
	tm1637_start();
	tm1637_write(TM1637_CMD_SET_ADDR | (position & (TM1637_POSITION_MAX - 1)));
	tm1637_write(sevent_segment_codes[spec & 0x0F] | (spec & 0x80));
	tm1637_stop();
}

void TM1637_set_brightness(const uint8_t value) {
	TM1637_send_config(_config & TM1637_SET_DISPLAY_ON,
			value & TM1637_BRIGHTNESS_MAX);
}

void TM1637_clear() {
	for (int i = 0; i < 4; i++)
		TM1637_display_segments(i, 0);
}

void TM1637_init() {
	TM1637_send_config(1, 3);
	TM1637_clear();
}
