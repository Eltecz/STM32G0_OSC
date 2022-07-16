/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc\user\sys.h
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

#define GPIO_MODE_IN    	0		//��ͨ����ģʽ
#define GPIO_MODE_OUT		1		//��ͨ���ģʽ
#define GPIO_MODE_AF		2		//AF����ģʽ
#define GPIO_MODE_AIN		3		//ģ������ģʽ

#define GPIO_SPEED_LOW		0		//GPIO�ٶ�(����,3M)
#define GPIO_SPEED_MID		1		//GPIO�ٶ�(����,15M)
#define GPIO_SPEED_FAST		2		//GPIO�ٶ�(����,60M)
#define GPIO_SPEED_HIGH		3		//GPIO�ٶ�(����,80M)  

#define GPIO_PUPD_NONE		0		//����������
#define GPIO_PUPD_PU		1		//����
#define GPIO_PUPD_PD		2		//����
#define GPIO_PUPD_RES		3		//���� 

#define GPIO_OTYPE_PP		0		//�������
#define GPIO_OTYPE_OD		1		//��©��� 

#define FTIR 1	//�½��ش���
#define RTIR 2	//�����ش���

u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllr);		//ϵͳʱ������
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllr); //ʱ�ӳ�ʼ�� 
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//����NVIC����
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//�����ж�
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//�ⲿ�ж����ú���(ֻ��GPIOA~I)
void NVIC_Clear(u8 BITx);//����ж�λ
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO���ú��� 
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);	//����ĳ��IO�ڵ����״̬
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx);				//��ȡĳ��IO�ڵ�����״̬
void GPIO_Set_withAF(GPIO_TypeDef* GPIOx,u32 BITx,u32 OTYPE,u32 OSPEED,u32 PUPD,u8 AFx);
#endif