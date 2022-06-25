/*
 * stm32f030k6_def.h
 *
 *  Created on: May 15, 2022
 *      Author: Duman
 */

#ifndef STM32F030K6_DEF_H_
#define STM32F030K6_DEF_H_


#include <stm32f030x6.h>

#define PIN_SET(PORT, PIN)			PORT->BSRR |= (1 << PIN)
#define PIN_CLR(PORT, PIN)			PORT->BRR |= (1 << PIN)
#define PIN_STATE(PORT, PIN)		((PORT->IDR & (1 << PIN)) ? 1 : 0)
#define PIN_OPEN_DRAIN(PORT, PIN)	PORT->OTYPER |= (1 << PIN)
#define PIN_PUSH_PULL(PORT, PIN)	PORT->OTYPER &= ~(1 << PIN)

#define GPIO_ON					RCC->AHBENR |= (RCC_AHBENR_GPIOAEN |	\
												RCC_AHBENR_GPIOBEN |	\
												RCC_AHBENR_GPIOCEN |	\
												RCC_AHBENR_GPIODEN)

#define TIMER_1_ON				RCC->APB2ENR |= RCC_APB2ENR_TIM1EN
#define TIMER_3_ON				RCC->APB1ENR |= RCC_APB1ENR_TIM3EN
#define TIMER_14_ON				RCC->APB1ENR |= RCC_APB1ENR_TIM14EN

#define USART_1_ON				RCC->APB2ENR |= RCC_APB2ENR_USART1EN
#define ADC_1_ON				RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

#define PIN_MODE_INPUT			0
#define PIN_MODE_OUTPUT			1
#define PIN_MODE_AF				2
#define PIN_MODE_ADC			3

#define PIN_PULL_NO				0
#define PIN_PULL_UP				1
#define PIN_PULL_DOWN			2

uint64_t SystemTickCntr;
#define get_timestamp()			SystemTickCntr

void adc_init();

void pin_set_mode(GPIO_TypeDef *port, uint16_t pin, uint8_t mode);
void pin_set_pull(GPIO_TypeDef *port, uint16_t pin, uint8_t pull);
void pin_set_function(GPIO_TypeDef *port, uint16_t pin, uint8_t function);
void pin_set_speed(GPIO_TypeDef *port, uint16_t pin, uint8_t speed);

void usart_init(uint16_t baudrate);
void usart_putc(uint8_t _c);
void usart_puts(char *_str);

void SystemTick_init();
void Stm32_Clock_Init(uint8_t PLL);

#endif /* STM32F030K6_DEF_H_ */
