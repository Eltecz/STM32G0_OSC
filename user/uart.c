#include "uart.h"
#include "stdio.h"	  

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#ifdef  __CC_ARM 
#pragma import(__use_no_semihosting)  
#elif defined ( __GNUC__ ) || defined (__clang__) 
__asm (".global __use_no_semihosting\n\t");
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//标准库需要的支持函数          
	struct __FILE 
	{ 
		int handle; 
		/* Whatever you require here. If the only file you are using is */ 
		/* standard output using printf() for debugging, no file handling */ 
		/* is required. */ 
	}; 
	/* FILE is typedef’ d in stdio.h. */ 
	FILE __stdout;   
#endif

//解决HAL库使用时,某些情况可能报错的bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
  
 
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART2->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->ISR&(1<<5))//接收到数据
	{	 
		res=USART1->RDR; 
/*		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} */
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (u8) res;      		
	} 
	USART1->ICR|=1<<3;	//清除溢出错误,否则可能会卡死在串口中断服务函数里面
} 
void USART2_IRQHandler(void)
{
	u8 res;	
	if(USART2->ISR&(1<<5))//接收到数据
	{	 
		res=USART2->RDR; 
/*		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} */
	while((USART2->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART2->TDR = (u8) res;      		
	} 
	USART2->ICR|=1<<3;	//清除溢出错误,否则可能会卡死在串口中断服务函数里面
} 
#endif										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//注意:USART1的时钟,是可以通过RCC_D2CCIP2R选择的
//但是我们一般用默认的选择即可(默认选择rcc_pclk2作为串口1时钟)
//pclk2即APB2的时钟频率,一般为100Mhz
//bound:波特率 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;	   
	temp=(pclk2*1000000)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->IOPENR|=1<<0;   	//使能PORTB口时钟  
	RCC->APBENR2|=1<<14;  	//使能串口1时钟 
	RCC->CCIPR|=1<<0;
	SYSCFG->CFGR1|=3<<3;//PA11,PA12->PA9,PA10
	GPIO_Set_withAF(GPIOA,PIN10|PIN9,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU,0);//PA9,PA10,复用功能,上拉输出 
	//波特率设置
 	USART1->BRR=temp; 		//波特率设置@OVER8=0 	
	USART1->CR1=0;		 	//清零CR1寄存器
	USART1->CR1|=0<<28;	 	//设置M1=0
	USART1->CR1|=0<<12;	 	//设置M0=0&M1=0,选择8位字长 
	USART1->CR1|=0<<15; 	//设置OVER8=0,16倍过采样 
	USART1->CR1|=1<<3;  	//串口发送使能 
//	USART1->ISR=0x6200e2;
#if EN_USART1_RX		  	//如果使能了接收
	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
#endif
	USART1->CR2|=1<<15;
	USART1->CR1|=1<<0;  	//串口使能
}
void uart1_sendonebyte(u8 dat)
{
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (u8) dat;      	
}
void uart2_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;	   
	temp=(pclk2*1000000)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->IOPENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APBENR1|=1<<17;  	//使能串口1时钟 
	#if defined(STM32G031xx)||defined(STM32G030xx)
	RCC->CCIPR|=1<<0;	
	#else
	RCC->CCIPR|=1<<2;
	#endif
	GPIO_Set_withAF(GPIOA,PIN2|PIN3,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU,1);//PA9,PA10,复用功能,上拉输出,AF10
	//波特率设置
 	USART2->BRR=temp; 		//波特率设置@OVER8=0 	
	USART2->CR1=0;		 	//清零CR1寄存器
	USART2->CR1|=0<<28;	 	//设置M1=0
	USART2->CR1|=0<<12;	 	//设置M0=0&M1=0,选择8位字长 
	USART2->CR1|=0<<15; 	//设置OVER8=0,16倍过采样 
	USART2->CR1|=1<<3;  	//串口发送使能 
//	USART2->ISR=0x6200e2;
#if EN_USART2_RX		  	//如果使能了接收
	//使能接收中断 
	USART2->CR1|=1<<2;  	//串口接收使能
	USART2->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//组2，最低优先级 
#endif
	USART2->CR1|=1<<0;  	//串口使能
}
void uart2_sendonebyte(u8 dat)
{
	while((USART2->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART2->TDR = (u8) dat;      	
}













