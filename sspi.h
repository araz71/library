/*
 * sspi.h
 *
 *  Created on: Aug 3, 2021
 *      Author: Duman
 */

#ifndef SSPI_H_
#define SSPI_H_

#include <stdint.h>
#include <def.h>

void sspi_init();
void sspi_cs(uint8_t _sel);
uint8_t sspi_write(uint8_t _c);
#endif /* SSPI_H_ */
