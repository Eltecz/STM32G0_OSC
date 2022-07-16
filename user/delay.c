#include "delay.h"
#include "sys.h"

static u16  fac_us=0;							//us延时倍乘数			   

//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟设置为内核时钟
//SYSCLK:系统时钟频率,即CPU频率(rcc_c_ck),400Mhz
void delay_init(u16 SYSCLK)
{
 	SysTick->CTRL|=1<<2;					//SYSTICK使用内核时钟源,同CPU同频率	 
	fac_us=SYSCLK;							//不论是否使用OS,fac_us都需要使用
}
//延时nus
//nus为要延时的us数.	
//注意:nus的值,不要大于41943us(最大值即2^24/fac_us@fac_us=400)
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL|=1<<0 ;          			//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~(1<<0) ;       			//关闭SYSTICK
	SysTick->VAL =0X00;       				//清空计数器 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=2^24 *1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对400M条件下,nms<=41.94ms 
void delay_xms(u16 nms)
{	 		  	  
	u32 temp=0;		   
	SysTick->LOAD=(u32)nms*fac_us*1000;		//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL|=1<<0 ;          			//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~(1<<0) ;       			//关闭SYSTICK
	SysTick->VAL =0X00;     		  		//清空计数器	  	    
} 
//延时nms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	u16 repeat=nms/30;						//这里用30,是考虑到可能有超频应用,
											//比如500Mhz的时候,delay_xms最大只能延时33.5ms左右了
	u16 remain=nms%30;
	while(repeat)
	{
		delay_xms(30);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
			 



































