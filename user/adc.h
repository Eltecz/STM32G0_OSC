/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc\user\adc.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-22 16:54:49
 */
#ifndef __ADC_H
#define __ADC_H	
#include "per_conf.h" 


#define ADC_CH5  		5 		 	//ͨ��5	   	     
extern  u16 adc_test_buf[adc_dma_length];

void Adc_Init(void); 				//ADC��ʼ��
u16  Get_Adc(u8 ch); 				//���ĳ��ͨ��ֵ 
u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
void  Adc_DMA_Init(u8 t,u8 ch);
#endif 















