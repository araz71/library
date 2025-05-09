/*
 * soft_i2c.h
 *
 *  Created on: Oct 6, 2018
 *      Author: amin
 */

#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_


#include <def.h>

//#define SOFT_I2C_DELAY			5


typedef struct {
	// Sets or clears SDA pin
	void (*sda_cntl)(bool_enu on);

	// Sets or clears SCL pin
	void (*scl_cntl)(bool_enu on);

	/*
	 * Sets SDA pin type.
	 *
	 * @param input If true then SDA sets to be input.
	*/
	void (*input_sda)(bool_enu input);

	/*
	 * Reads SDA pin
	 *
	 * @return '1' if SDA is in high state. Otherwise returns '0'.
	*/
	uint8_t (*read_sda)();
} SoftI2cCallbacks;

// Sets SDA and SCL
void soft_i2c_init();

void soft_i2c_start(SoftI2cCallbacks* i2c_ifw);
uint8_t soft_i2c_write(SoftI2cCallbacks* i2c_ifw, uint8_t _data);
void soft_i2c_restart(SoftI2cCallbacks* i2c_ifw);
void soft_i2c_stop(SoftI2cCallbacks* i2c_ifw);
uint8_t soft_i2c_read(SoftI2cCallbacks* i2c_ifw, uint8_t _ack);

#endif /* SOFT_I2C_H_ */
