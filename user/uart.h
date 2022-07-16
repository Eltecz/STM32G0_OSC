/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc_v1\user\uart.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-07 23:23:55
 */
#ifndef __USART_H
#define __USART_H 
#include "per_conf.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//����1��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/4/21
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��  
//��
////////////////////////////////////////////////////////////////////////////////// 


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
//#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart1_init(u32 pclk2,u32 bound); 
void uart1_sendonebyte(u8 dat);//����һ���ֽ�
void uart2_init(u32 pclk2,u32 bound); 
void uart2_sendonebyte(u8 dat);//����һ���ֽ�
#endif	   
















