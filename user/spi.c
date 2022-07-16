/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc\user\spi.c
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
	RCC->IOPENR|=1<<0;			//ʹ��PORTAʱ�� 
	RCC->APBENR2|=1<<12;		//SPI1ʱ��ʹ�� 
	GPIO_Set_withAF(GPIOA,PIN1|PIN2,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU,0);	//PB13~15���ù������	
	//����ֻ���SPI�ڳ�ʼ��
	RCC->APBRSTR2|=1<<12;		//��λSPI1
	RCC->APBRSTR2&=~(1<<12);	//ֹͣ��λSPI1
		
    SPI1->CR1|=1<<9;            //SSM=1,�������NSS��
	SPI1->CR1|=1<<8;			//SSI=1,�����ڲ�SS�ź�Ϊ�ߵ�ƽ	
	SPI1->CR1|=0<<3;			//BR[2:0]=7,����spi_ker_ckΪ256��Ƶ.   

    SPI1->CR1|=7;               //[MSTR,CPOL,CPHA]=[1,1,1]

	SPI1->CR2|=7<<8;			//DSIZE[4:0]=7,����SPI֡��ʽΪ8λ,���ֽڴ��� 

	SPI1->I2SCFGR&=~(11<<0);		//ѡ��SPIģʽ
	SPI1->CR1|=1<<6;			//SPE=1,ʹ��SPI1
 
	SPI1_WriteByte(0xff);	//��������		 
}   

//SPI1�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=spi_ker_ck/2^(SpeedSet+1)
//spi_ker_ck����ѡ������pll1_q_ck,Ϊ200Mhz
void SPI1_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;					//���Ʒ�Χ
 	SPI1->CR1&=~(1<<0); 			//SPE=0,SPI�豸ʧ��
	SPI1->CR1&=~(7<<3); 			//MBR[2:0]=0,���ԭ���ķ�Ƶ����
	SPI1->CR1|=(u32)SpeedSet<<3;	//MBR[2:0]=SpeedSet,����SPI1�ٶ�  
	SPI1->CR1|=1<<6; 				//SPE=1,SPI�豸ʹ��	 	  
} 

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
void SPI1_WriteByte(u8 TxData)
{			 	   
	while (!(SPI1->SR & (1<<1)));
	*(u8*)&(SPI1->DR)=(u8)TxData;
	while ((SPI1->SR & 6272));//fifo tx& ovr
}


