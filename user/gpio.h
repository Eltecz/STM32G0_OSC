/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\gpio.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:37:08
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-06-21 16:07:02
 */
/*
 * @Description: 
 * @FilePath: \osc\user\gpio.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:37:08
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-21 15:39:01
 */
#ifndef __GPIO_H__
#define __GPIO_H__

#include "per_conf.h"

void en_rcc();
void gpio_init();

#define LED_ON()  GPIOA->BSRR|=1<<7
#define LED_OFF() GPIOA->BSRR|=1<<23
#define KL()    ((GPIOC->IDR)&((1<<14)))
#define KR()    ((GPIOC->IDR)&((1<<15)))
#define KM()    ((GPIOB->IDR)&((1<<7)))
#endif