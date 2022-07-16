/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1\user\adc.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-13 17:27:54
 */
#include "adc.h"	 

u16 adc_test_buf[adc_dma_length]={0};
//初始化ADC
//这里我们仅以规则通道为例
//我们默认仅开启ADC1_CH5																	   
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->APBENR2|=1<<20;    	//使能ADC1时钟 
	RCC->IOPENR|=1<<0;   	//使能PORTA口时钟  
	RCC->CCIPR&=~(3<<30);//SYSTICK作为ADC时钟
	GPIO_Set(GPIOA,PIN12,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);	//PA5,模拟输入,下拉   

	RCC->APBRSTR2|=1<<20;   	//ADCs复位
	RCC->APBRSTR2&=~(1<<20);	//复位结束	 
	ADC->CCR=0<<18;			//ADCCLK=SYSTICK/1=64M

	ADC1->CR=0;   			//CR1设置清零

	ADC1->CFGR1=0;
	ADC1->CFGR1|=0<<3;      	//12位模式
	
	ADC1->CFGR1&=~(1<<13);    	//单次转换模式
 	ADC1->CFGR1&=~(1<<5);   	//右对齐	
	ADC1->CFGR1&=~(1<<10);    	//软件触发
	//PA5,ADC1_CH5_IN，采样时间为1.5ADC周期
	ADC1->SMPR=0;
	ADC1->CHSELR=0;

	ADC1->CHSELR|=1<<5;//1个转换在规则序列中 也就是只转换规则序列5			   
	//设置通道5的采样时间
 	ADC1->SMPR|=(2<<4); 	//通道5  480个周期,提高采样时间可以提高精确度	 
	ADC1->SMPR|=1<<13;
 	ADC1->CR|=1<<0;	   	//开启AD转换器	  	
	
	ADC1->CR|=1<<31;		//校准ADC
	while(!(ADC1->CR&(1<<31)));//等待转换结束	
}		

void  Adc_DMA_Init(u8 t,u8 ch)
{    
 	RCC->APBENR2|=1<<20;    	//使能ADC1时钟 
	ADC_DMA_CHNNAL_INIT();
	TIM1_adc_Init(t);
	//先初始化IO口

	RCC->IOPENR|=1<<0;   	//使能PORTA口时钟  
	RCC->CCIPR&=~(3<<30);//SYSTICK作为ADC时钟
//	RCC->CCIPR|=1<<30;
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);	//PA5,模拟输入,下拉   
	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);	//PA5,模拟输入,下拉   
	RCC->APBRSTR2|=1<<20;   	//ADCs复位
	RCC->APBRSTR2&=~(1<<20);	//复位结束	 
	ADC->CCR=0<<18;			//ADCCLK=SYSTICK/1=64M
	ADC1->CR=0;   			//CR1设置清零

	ADC1->CFGR1=0;
	ADC1->CFGR1|=0<<3;      	//12位模式
	
	ADC1->CFGR1&=~(1<<13);    	//单次转换模式
//	ADC1->CFGR1|=(1<<13);    	//单次转换模式
 	ADC1->CFGR1&=~(1<<5);   	//右对齐	
//	ADC1->CFGR1&=~(1<<10);    	//软件触发
	ADC1->CFGR1|=(1<<10);    	//硬件触发
//	ADC1->CFGR1|=(1<<6);
	ADC1->CFGR1|=1;				//使能DMA
	//PA5,ADC1_CH5_IN，采样时间为1.5ADC周期
	ADC1->SMPR=0;
	ADC1->CHSELR=0;
//	ADC1->CHSELR|=1<<0;//1个转换在规则序列中 也就是只转换规则序列5		
	ADC1->CHSELR=1<<(ch?3:0);//1个转换在规则序列中 也就是只转换规则序列5			   
	//设置通道5的采样时间
	ADC1->SMPR=0;
 	ADC1->SMPR|=(2<<4); 	//通道5  480个周期,提高采样时间可以提高精确度	 
//	ADC1->SMPR|=1<<8;
	ADC1->SMPR|=1<<(ch?11:8);
 	ADC1->CR|=1<<0;	   	//开启AD转换器	  	
	
	ADC1->CR|=1<<31;		//校准ADC
	while(!(ADC1->CR&(1<<31)));//等待转换结束	
//	ADC1->CHSELR|=1<<0;
	ADC1->CHSELR=1<<(ch?3:0);//1个转换在规则序列中 也就是只转换规则序列5
	ADC1->SMPR|=1<<24;
}	

//获得ADC值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->CHSELR=0;//规则序列1 通道ch
	ADC1->CHSELR|=1<<ch;
	ADC1->SMPR|=1<<(ch+8);
	ADC1->CR|=1<<2;       //启动规则转换通道 
	while(!(ADC1->CR&1<<2));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
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









