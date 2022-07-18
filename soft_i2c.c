/*
 * soft_i2c.c
 *
 *  Created on: Oct 6, 2018
 *      Author: amin
 */

#include "soft_i2c.h"

#define SOFT_SDA_ON		PIN_SET(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN)
#define SOFT_SDA_OFF	PIN_CLR(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN)

#define SOFT_SCL_ON		PIN_SET(SOFT_I2C_SCL_GPIO, SOFT_I2C_SCL_PIN)
#define SOFT_SCL_OFF	PIN_CLR(SOFT_I2C_SCL_GPIO, SOFT_I2C_SCL_PIN)

void soft_i2c_init() {
	pin_set_mode(SOFT_I2C_SCL_GPIO, SOFT_I2C_SCL_PIN, PIN_MODE_OUTPUT);
	pin_set_mode(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, PIN_MODE_OUTPUT);
	pin_set_speed(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, 1);
	pin_set_speed(SOFT_I2C_SCL_GPIO, SOFT_I2C_SCL_PIN, 1);

	PIN_PUSH_PULL(SOFT_I2C_SCL_GPIO, SOFT_I2C_SCL_PIN);
	PIN_OPEN_DRAIN(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN);

	SOFT_SCL_ON;
	SOFT_SDA_ON;
	for (int i = 0; i < 100; i++)
		__NOP();


}
void soft_i2c_start() {
	SOFT_SCL_ON;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SDA_ON;

	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	SOFT_SDA_OFF;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	SOFT_SCL_OFF;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

uint8_t soft_i2c_write(uint8_t _data) {
	uint8_t data = _data;

	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80)
			SOFT_SDA_ON;
		else
			SOFT_SDA_OFF;
		data = data << 1;
		i2c_clk();
	}
	SOFT_SDA_ON;
	pin_set_mode(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, PIN_MODE_INPUT);
	i2c_clk();
	uint8_t ack = PIN_STATE(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN) ? 0 : 1;
	pin_set_mode(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, PIN_MODE_OUTPUT);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	return ack;
}

void soft_i2c_restart() {
	SOFT_SDA_ON;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SCL_ON;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SDA_OFF;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SCL_OFF;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

void soft_i2c_stop() {
	SOFT_SDA_OFF;

	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	SOFT_SCL_OFF;

	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	SOFT_SCL_ON;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SDA_ON;

	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

void i2c_clk() {
	SOFT_SCL_ON;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
	SOFT_SCL_OFF;
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

uint8_t soft_i2c_read(uint8_t _ack) {
	uint8_t x = 0;

	pin_set_mode(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, PIN_MODE_INPUT);
	for (uint8_t i = 0; i < 7; i++) {
		x |= (PIN_STATE(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN) ? 1 : 0);
		x = x << 1;
		i2c_clk();
	}

	x |= (PIN_STATE(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN) ? 1 : 0);
	i2c_clk();

	pin_set_mode(SOFT_I2C_SDA_GPIO, SOFT_I2C_SDA_PIN, PIN_MODE_OUTPUT);

	if (_ack)
		SOFT_SDA_ON;
	else
		SOFT_SDA_OFF;

	i2c_clk();

	return x;
}
