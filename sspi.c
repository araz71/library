/*
 * sspi.c
 *
 *  Created on: Aug 3, 2021
 *      Author: Duman
 */


#include <sspi.h>

void sspi_init() {
	GPIO_InitTypeDef gp;
	gp.GPIO_Speed = GPIO_Speed_10MHz;

	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	gp.GPIO_Pin = MMC_MOSI_PIN;
	GPIO_Init(MMC_MOSI_PORT, &gp);

	gp.GPIO_Pin = MMC_CS_PIN;
	GPIO_Init(MMC_CS_PORT, &gp);

	gp.GPIO_Pin = MMC_SCK_PIN;
	GPIO_Init(MMC_SCK_PORT, &gp);

	gp.GPIO_Pin = MMC_MISO_PIN;
	gp.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(MMC_MISO_PORT, &gp);

	gp.GPIO_Pin = GPIO_Pin_3;
	gp.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gp);

	GPIO_WriteBit(MMC_MOSI_PORT, MMC_MOSI_PIN, 0);
	GPIO_WriteBit(MMC_SCK_PORT, MMC_SCK_PIN, 0);

	sspi_cs(0);
}
uint8_t sspi_write(uint8_t _c) {
	uint8_t v = 0;
	uint8_t mask = 0x80;
	for (int i = 0 ; i < 8; i++) {
		v = v << 1;
		if (_c & mask) {
			MMC_MOSI_PORT->BSRR |= MMC_MOSI_PIN;
		} else {
			MMC_MOSI_PORT->BRR |= MMC_MOSI_PIN;
		}
		MMC_SCK_PORT->BSRR |= MMC_SCK_PIN;
		__NOP();
		if (MMC_MISO_PORT->IDR & MMC_MISO_PIN) {
			v |= 1;
		}
		MMC_SCK_PORT->BRR |= MMC_SCK_PIN;
		__NOP();
		mask = mask >> 1;
	}
	return v;
}
void sspi_cs(uint8_t _sel) {
	if (_sel) {
		MMC_CS_PORT->BRR |= MMC_CS_PIN;
	} else {
		MMC_CS_PORT->BSRR |= MMC_CS_PIN;
	}
	for (int j = 0; j < 0xef; j++);
}
