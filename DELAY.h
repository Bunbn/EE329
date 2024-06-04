/*
 * DELAY.h
 *
 *  Created on: Apr 20, 2024
 *      Author: adash
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_
#include "main.h"
//#define SysTick GPIOB //change this

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
