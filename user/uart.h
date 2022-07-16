/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1\user\uart.h
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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//串口1初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/4/21
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明  
//无
////////////////////////////////////////////////////////////////////////////////// 


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
//#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart1_init(u32 pclk2,u32 bound); 
void uart1_sendonebyte(u8 dat);//发送一个字节
void uart2_init(u32 pclk2,u32 bound); 
void uart2_sendonebyte(u8 dat);//发送一个字节
#endif	   
















