/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc_v1\user\adc.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-13 17:27:54
 */
#include "adc.h"	 

u16 adc_test_buf[adc_dma_length]={0};
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ADC1_CH5																	   
void  Adc_Init(void)
{    
	//�ȳ�ʼ��IO��
 	RCC->APBENR2|=1<<20;    	//ʹ��ADC1ʱ�� 
	RCC->IOPENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->CCIPR&=~(3<<30);//SYSTICK��ΪADCʱ��
	GPIO_Set(GPIOA,PIN12,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);	//PA5,ģ������,����   

	RCC->APBRSTR2|=1<<20;   	//ADCs��λ
	RCC->APBRSTR2&=~(1<<20);	//��λ����	 
	ADC->CCR=0<<18;			//ADCCLK=SYSTICK/1=64M

	ADC1->CR=0;   			//CR1��������

	ADC1->CFGR1=0;
	ADC1->CFGR1|=0<<3;      	//12λģʽ
	
	ADC1->CFGR1&=~(1<<13);    	//����ת��ģʽ
 	ADC1->CFGR1&=~(1<<5);   	//�Ҷ���	
	ADC1->CFGR1&=~(1<<10);    	//�������
	//PA5,ADC1_CH5_IN������ʱ��Ϊ1.5ADC����
	ADC1->SMPR=0;
	ADC1->CHSELR=0;

	ADC1->CHSELR|=1<<5;//1��ת���ڹ��������� Ҳ����ֻת����������5			   
	//����ͨ��5�Ĳ���ʱ��
 	ADC1->SMPR|=(2<<4); 	//ͨ��5  480������,��߲���ʱ�������߾�ȷ��	 
	ADC1->SMPR|=1<<13;
 	ADC1->CR|=1<<0;	   	//����ADת����	  	
	
	ADC1->CR|=1<<31;		//У׼ADC
	while(!(ADC1->CR&(1<<31)));//�ȴ�ת������	
}		

void  Adc_DMA_Init(u8 t,u8 ch)
{    
 	RCC->APBENR2|=1<<20;    	//ʹ��ADC1ʱ�� 
	ADC_DMA_CHNNAL_INIT();
	TIM1_adc_Init(t);
	//�ȳ�ʼ��IO��

	RCC->IOPENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->CCIPR&=~(3<<30);//SYSTICK��ΪADCʱ��
//	RCC->CCIPR|=1<<30;
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);	//PA5,ģ������,����   
	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);	//PA5,ģ������,����   
	RCC->APBRSTR2|=1<<20;   	//ADCs��λ
	RCC->APBRSTR2&=~(1<<20);	//��λ����	 
	ADC->CCR=0<<18;			//ADCCLK=SYSTICK/1=64M
	ADC1->CR=0;   			//CR1��������

	ADC1->CFGR1=0;
	ADC1->CFGR1|=0<<3;      	//12λģʽ
	
	ADC1->CFGR1&=~(1<<13);    	//����ת��ģʽ
//	ADC1->CFGR1|=(1<<13);    	//����ת��ģʽ
 	ADC1->CFGR1&=~(1<<5);   	//�Ҷ���	
//	ADC1->CFGR1&=~(1<<10);    	//�������
	ADC1->CFGR1|=(1<<10);    	//Ӳ������
//	ADC1->CFGR1|=(1<<6);
	ADC1->CFGR1|=1;				//ʹ��DMA
	//PA5,ADC1_CH5_IN������ʱ��Ϊ1.5ADC����
	ADC1->SMPR=0;
	ADC1->CHSELR=0;
//	ADC1->CHSELR|=1<<0;//1��ת���ڹ��������� Ҳ����ֻת����������5		
	ADC1->CHSELR=1<<(ch?3:0);//1��ת���ڹ��������� Ҳ����ֻת����������5			   
	//����ͨ��5�Ĳ���ʱ��
	ADC1->SMPR=0;
 	ADC1->SMPR|=(2<<4); 	//ͨ��5  480������,��߲���ʱ�������߾�ȷ��	 
//	ADC1->SMPR|=1<<8;
	ADC1->SMPR|=1<<(ch?11:8);
 	ADC1->CR|=1<<0;	   	//����ADת����	  	
	
	ADC1->CR|=1<<31;		//У׼ADC
	while(!(ADC1->CR&(1<<31)));//�ȴ�ת������	
//	ADC1->CHSELR|=1<<0;
	ADC1->CHSELR=1<<(ch?3:0);//1��ת���ڹ��������� Ҳ����ֻת����������5
	ADC1->SMPR|=1<<24;
}	

//���ADCֵ
//ch:ͨ��ֵ 0~16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	//����ת������	  		 
	ADC1->CHSELR=0;//��������1 ͨ��ch
	ADC1->CHSELR|=1<<ch;
	ADC1->SMPR|=1<<(ch+8);
	ADC1->CR|=1<<2;       //��������ת��ͨ�� 
	while(!(ADC1->CR&1<<2));//�ȴ�ת������	 	   
	return ADC1->DR;		//����adcֵ	
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}  









