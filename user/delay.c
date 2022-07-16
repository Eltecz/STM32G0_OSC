#include "delay.h"
#include "sys.h"

static u16  fac_us=0;							//us��ʱ������			   

//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
//SYSTICK��ʱ������Ϊ�ں�ʱ��
//SYSCLK:ϵͳʱ��Ƶ��,��CPUƵ��(rcc_c_ck),400Mhz
void delay_init(u16 SYSCLK)
{
 	SysTick->CTRL|=1<<2;					//SYSTICKʹ���ں�ʱ��Դ,ͬCPUͬƵ��	 
	fac_us=SYSCLK;							//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
}
//��ʱnus
//nusΪҪ��ʱ��us��.	
//ע��:nus��ֵ,��Ҫ����41943us(���ֵ��2^24/fac_us@fac_us=400)
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 				//ʱ�����	  		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL|=1<<0 ;          			//��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~(1<<0) ;       			//�ر�SYSTICK
	SysTick->VAL =0X00;       				//��ռ����� 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=2^24 *1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��400M������,nms<=41.94ms 
void delay_xms(u16 nms)
{	 		  	  
	u32 temp=0;		   
	SysTick->LOAD=(u32)nms*fac_us*1000;		//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL|=1<<0 ;          			//��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~(1<<0) ;       			//�ر�SYSTICK
	SysTick->VAL =0X00;     		  		//��ռ�����	  	    
} 
//��ʱnms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
	u16 repeat=nms/30;						//������30,�ǿ��ǵ������г�ƵӦ��,
											//����500Mhz��ʱ��,delay_xms���ֻ����ʱ33.5ms������
	u16 remain=nms%30;
	while(repeat)
	{
		delay_xms(30);
		repeat--;
	}
	if(remain)delay_xms(remain);
} 
			 



































