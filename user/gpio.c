/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\gpio.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:37:15
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-08 23:40:55
 */
#include "gpio.h"

void en_rcc()
{
	RCC->IOPENR|=1<<0;
	RCC->IOPENR|=1<<1;
	RCC->IOPENR|=1<<2;
}
void gpio_init()
{
	GPIO_Set(GPIOA,PIN7,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_NONE); 
//	GPIO_Set(GPIOA,PIN0,GPIO_MODE_IN,GPIO_OTYPE_OD,GPIO_SPEED_FAST,GPIO_PUPD_PU); 
	GPIO_Set(GPIOC,PIN14|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	GPIO_Set(GPIOB,PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
//	Ex_NVIC_Config(GPIO_A,0,FTIR);
//	MY_NVIC_Init(3,2,EXTI0_1_IRQn,2);    
}