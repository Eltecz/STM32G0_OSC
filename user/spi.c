/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc\user\spi.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 17:01:59
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-22 10:34:25
 */
#include "spi.h"

void SPI1_Init(void)
{	 
	u32 tempreg=0;
	RCC->IOPENR|=1<<0;			//使能PORTA时钟 
	RCC->APBENR2|=1<<12;		//SPI1时钟使能 
	GPIO_Set_withAF(GPIOA,PIN1|PIN2,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU,0);	//PB13~15复用功能输出	
	//这里只针对SPI口初始化
	RCC->APBRSTR2|=1<<12;		//复位SPI1
	RCC->APBRSTR2&=~(1<<12);	//停止复位SPI1
		
    SPI1->CR1|=1<<9;            //SSM=1,软件管理NSS脚
	SPI1->CR1|=1<<8;			//SSI=1,设置内部SS信号为高电平	
	SPI1->CR1|=0<<3;			//BR[2:0]=7,设置spi_ker_ck为256分频.   

    SPI1->CR1|=7;               //[MSTR,CPOL,CPHA]=[1,1,1]

	SPI1->CR2|=7<<8;			//DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输 

	SPI1->I2SCFGR&=~(11<<0);		//选择SPI模式
	SPI1->CR1|=1<<6;			//SPE=1,使能SPI1
 
	SPI1_WriteByte(0xff);	//启动传输		 
}   

//SPI1速度设置函数
//SpeedSet:0~7
//SPI速度=spi_ker_ck/2^(SpeedSet+1)
//spi_ker_ck我们选择来自pll1_q_ck,为200Mhz
void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;					//限制范围
 	SPI1->CR1&=~(1<<0); 			//SPE=0,SPI设备失能
	SPI1->CR1&=~(7<<3); 			//MBR[2:0]=0,清除原来的分频设置
	SPI1->CR1|=(u32)SpeedSet<<3;	//MBR[2:0]=SpeedSet,设置SPI1速度  
	SPI1->CR1|=1<<6; 				//SPE=1,SPI设备使能	 	  
} 

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
void SPI1_WriteByte(u8 TxData)
{			 	   
	while (!(SPI1->SR & (1<<1)));
	*(u8*)&(SPI1->DR)=(u8)TxData;
	while ((SPI1->SR & 6272));//fifo tx& ovr
}


