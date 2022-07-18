/*
 * soft_i2c.h
 *
 *  Created on: Oct 6, 2018
 *      Author: amin
 */

#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_


#include <def.h>

#define SOFT_I2C_DELAY			5

#define I2C_START()				soft_i2c_start()
#define I2C_STOP()				soft_i2c_stop()
#define I2C_WRITE(X)			soft_i2c_write(X)
#define I2C_READ(X)				soft_i2c_read(X)
#define I2C_RESTART()			soft_i2c_restart()

void soft_i2c_init();
void soft_i2c_start();
uint8_t soft_i2c_write(uint8_t _data);
void soft_i2c_restart();
void soft_i2c_stop();
uint8_t soft_i2c_read(uint8_t _ack);
void i2c_clk() ;

#endif /* SOFT_I2C_H_ */
