/*
 * soft_i2c.c
 *
 *  Created on: Oct 6, 2018
 *      Author: amin
 */

#include <soft_i2c.h>

#define SOFT_I2C_DELAY			1

static void i2c_clk(SoftI2cCallbacks* i2c_ifw);

void soft_i2c_init(SoftI2cCallbacks* i2c_ifw) {
	i2c_ifw->input_sda(0);

	i2c_ifw->sda_cntl(1);
	i2c_ifw->scl_cntl(1);

	for (int i = 0; i < 100; i++)
		__NOP();
}

void soft_i2c_start(SoftI2cCallbacks* i2c_ifw) {
	/*
	In start SDA should go low after SCL goes low

	SDA --------|
				|__________
	
	SCL -----|
			 |______________
	*/

	// First make sure I2C is in idle state
	i2c_ifw->scl_cntl(TRUE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->sda_cntl(TRUE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->sda_cntl(FALSE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->scl_cntl(FALSE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

uint8_t soft_i2c_write(SoftI2cCallbacks* i2c_ifw, uint8_t _data) {
	uint8_t data = _data;

	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80) {
			i2c_ifw->sda_cntl(TRUE);
		} else {
			i2c_ifw->sda_cntl(FALSE);
		}
		data = data << 1;

		i2c_clk(i2c_ifw);
	}

	i2c_ifw->sda_cntl(TRUE);

	// Read NACK. First SDA should be input
	i2c_ifw->input_sda(TRUE);

	i2c_ifw->scl_cntl(TRUE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++) {
		__NOP();
	}

	uint8_t ack = i2c_ifw->read_sda();
	i2c_ifw->input_sda(FALSE);

	i2c_ifw->scl_cntl(FALSE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++) {
		__NOP();
	}

	return ack;
}

void soft_i2c_restart(SoftI2cCallbacks* i2c_ifw) {
	i2c_ifw->sda_cntl(TRUE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->scl_cntl(TRUE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->sda_cntl(FALSE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->scl_cntl(FALSE);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

void soft_i2c_stop(SoftI2cCallbacks* i2c_ifw) {
	/*
	SDA 		   |--------
		___________|
	
	SCL		  |--------------
		______|
	
	*/
	i2c_ifw->sda_cntl(OFF);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->scl_cntl(ON);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->sda_cntl(ON);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

static void i2c_clk(SoftI2cCallbacks* i2c_ifw) {
	i2c_ifw->scl_cntl(ON);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();

	i2c_ifw->scl_cntl(OFF);
	for (int i = 0; i < SOFT_I2C_DELAY; i++)
		__NOP();
}

uint8_t soft_i2c_read(SoftI2cCallbacks* i2c_ifw, uint8_t _ack) {
	uint8_t x = 0;

	i2c_ifw->input_sda(TRUE);
	for (uint8_t i = 0; i < 7; i++) {
		x |= i2c_ifw->read_sda();
		x = x << 1;

		i2c_clk(i2c_ifw);
	}

	x |= i2c_ifw->read_sda();
	i2c_clk(i2c_ifw);

	i2c_ifw->input_sda(FALSE);
	if (_ack)
		i2c_ifw->sda_cntl(ON);
	else
		i2c_ifw->sda_cntl(OFF);

	i2c_clk(i2c_ifw);

	return x;
}
