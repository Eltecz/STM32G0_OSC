/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1\user\dma.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-22 11:28:06
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-09 10:15:08
 */
#include "dma.h"
/*
SPI1->DMA   DMA_CHNNAL  1
ADC->DMA    DMA_CHNNAL  2
*/
extern u16 fill_color_buf;
extern  u16 adc_test_buf[adc_dma_length];

DMA_Channel_TypeDef *SPI1_LCD_DMA=DMA1_Channel1;
DMAMUX_Channel_TypeDef  *SPI1_LCD_DMA_MUX=DMAMUX1_Channel0;

DMA_Channel_TypeDef *ADC_DMA=DMA1_Channel2;
DMAMUX_Channel_TypeDef  *ADC_DMA_MUX=DMAMUX1_Channel1;

void SPI1_LCD_DMA_INIT()
{

	u8 streamx=1;
 	RCC->AHBENR|=1<<0;//DMA1时钟使能 
	while(SPI1_LCD_DMA->CCR&0X01);//等待DMA可配置 
	DMA1->IFCR|=0X07;//清空之前该stream上的所有中断标志
	
	SPI1_LCD_DMA->CPAR=(u32)&SPI1->DR;		//DMA外设地址
	SPI1_LCD_DMA->CMAR=(u32)&fill_color_buf;//DMA 存储器0地址
	SPI1_LCD_DMA->CNDTR=0;	//DMA 传输数据长度
	SPI1_LCD_DMA->CCR=0;			//先全部复位CR寄存器值 
	
	SPI1_LCD_DMA->CCR|=1<<4;		//存储器到外设模式
	SPI1_LCD_DMA->CCR|=1<<5;		//非循环模式(即使用普通模式)
	SPI1_LCD_DMA->CCR|=0<<6;		//外设非增量模式
	SPI1_LCD_DMA->CCR|=0<<7;		//存储器非增量模式
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1_LCD_DMA->CCR|=1<<8;		//外设数据长度:8位
#else
	SPI1_LCD_DMA->CCR|=0<<8;		//外设数据长度:8位
#endif
	SPI1_LCD_DMA->CCR|=1<<10;		//存储器数据长度:8位
	SPI1_LCD_DMA->CCR|=1<<12;		//中等优先级
	SPI1_LCD_DMA_MUX->CCR|=(u32)17;//通道选择
	//DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
}
void SPI1_DMA_START(u32 ndtr)
{
	SPI1_LCD_DMA->CCR&=~(1<<0); 	//关闭DMA传输 
	while(SPI1_LCD_DMA->CCR&0X1);	//确保DMA可以被设置  
	SPI1_LCD_DMA->CNDTR=ndtr;		//DMA 传输数据长度
	SPI1_LCD_DMA->CCR|=1<<0;		//开启DMA传输
}	

void ADC_DMA_CHNNAL_INIT()
{
	RCC->AHBENR|=1<<0;//DMA1时钟使能 
	ADC_DMA->CCR&=~(1<<0); 	//关闭DMA传输 
	while(ADC_DMA->CCR&0X01);//等待DMA可配置 
	DMA1->IFCR|=0X38;//清空之前该stream上的所有中断标志
	
	ADC_DMA->CPAR=(u32)&ADC1->DR;		//DMA外设地址
	ADC_DMA->CMAR=(u32)&adc_test_buf;//DMA 存储器0地址
	ADC_DMA->CNDTR=0;	//DMA 传输数据长度
	ADC_DMA->CCR=0;			//先全部复位CR寄存器值 
    
//    ADC_DMA->CCR|=1<<1;     //DMA中断
	ADC_DMA->CCR|=0<<4;		//外设到存储器模式
	ADC_DMA->CCR|=1<<5;		//非循环模式(即使用普通模式)
	ADC_DMA->CCR|=0<<6;		//外设非增量模式
	ADC_DMA->CCR|=1<<7;		//存储器增量模式
	ADC_DMA->CCR|=1<<8;		//外设数据长度:16位
	ADC_DMA->CCR|=1<<10;		//存储器数据长度:16位
	ADC_DMA->CCR|=1<<12;		//中等优先级
	ADC_DMA_MUX->CCR|=(u32)5;//通道选择

}
void ADC_DMA_START(u32 ndtr)
{
	ADC_DMA->CCR&=~(1<<0); 	//关闭DMA传输 
	while(ADC_DMA->CCR&0X1);	//确保DMA可以被设置  
	ADC_DMA->CNDTR=ndtr;		//DMA 传输数据长度
	ADC1->CR|=1<<2;       //启动规则转换通道 
	ADC_DMA->CCR|=1<<0;		//开启DMA传输
	TIM1->CR1|=0x01;    //使能定时器3
	TIM1->CCER|=1<<12;
	while (!((DMA1->ISR & 0x10)));
	DMA1->IFCR |= 0x10;
	TIM1->CR1-=0x01;    //使能定时器3
	TIM1->CCER-=1<<12;
}	
//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:0~7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  

void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{ 
	DMA_TypeDef *DMAx=DMA1;
	u8 streamx;
 	RCC->AHBENR|=1<<21;//DMA1时钟使能 
	while(DMA_Streamx->CCR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X8)/0X14;		//得到stream通道号
	DMAx->IFCR|=0X07<<(3*(streamx));//清空之前该stream上的所有中断标志
	
	DMA_Streamx->CPAR=par;		//DMA外设地址
	DMA_Streamx->CMAR=mar;		//DMA 存储器0地址
	DMA_Streamx->CNDTR=ndtr;	//DMA 传输数据长度
	DMA_Streamx->CCR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CCR|=1<<6;		//存储器到外设模式
	DMA_Streamx->CCR|=0<<8;		//非循环模式(即使用普通模式)
	DMA_Streamx->CCR|=0<<9;		//外设非增量模式
	DMA_Streamx->CCR|=1<<10;		//存储器增量模式
	DMA_Streamx->CCR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CCR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CCR|=1<<16;		//中等优先级
	DMA_Streamx->CCR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CCR|=0<<23;		//存储器突发单次传输
	DMA_Streamx->CCR|=(u32)chx<<25;//通道选择
	//DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
} 
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CCR&=~(1<<0); 	//关闭DMA传输 
	while(DMA_Streamx->CCR&0X1);	//确保DMA可以被设置  
	DMA_Streamx->CNDTR=ndtr;		//DMA 存储器0地址 
	DMA_Streamx->CCR|=1<<0;		//开启DMA传输
}	  
