/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc\user\sys.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-22 19:53:12
 */
#ifndef __SYS_H
#define __SYS_H	 

#include "stm32g0xx.h"

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2

#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 

#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_LOW		0		//GPIO速度(低速,3M)
#define GPIO_SPEED_MID		1		//GPIO速度(中速,15M)
#define GPIO_SPEED_FAST		2		//GPIO速度(快速,60M)
#define GPIO_SPEED_HIGH		3		//GPIO速度(高速,80M)  

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

#define FTIR 1	//下降沿触发
#define RTIR 2	//上升沿触发

u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllr);		//系统时钟设置
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllr); //时钟初始化 
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//设置NVIC分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//外部中断配置函数(只对GPIOA~I)
void NVIC_Clear(u8 BITx);//清除中断位
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO设置函数 
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);	//设置某个IO口的输出状态
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx);				//读取某个IO口的输入状态
void GPIO_Set_withAF(GPIO_TypeDef* GPIOx,u32 BITx,u32 OTYPE,u32 OSPEED,u32 PUPD,u8 AFx);
#endif