#include "uart.h"
#include "stdio.h"	  

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#ifdef  __CC_ARM 
#pragma import(__use_no_semihosting)  
#elif defined ( __GNUC__ ) || defined (__clang__) 
__asm (".global __use_no_semihosting\n\t");
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//��׼����Ҫ��֧�ֺ���          
	struct __FILE 
	{ 
		int handle; 
		/* Whatever you require here. If the only file you are using is */ 
		/* standard output using printf() for debugging, no file handling */ 
		/* is required. */ 
	}; 
	/* FILE is typedef�� d in stdio.h. */ 
	FILE __stdout;   
#endif

//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
  
 
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART2->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void USART1_IRQHandler(void)
{
	u8 res;	
	if(USART1->ISR&(1<<5))//���յ�����
	{	 
		res=USART1->RDR; 
/*		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		} */
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (u8) res;      		
	} 
	USART1->ICR|=1<<3;	//����������,������ܻῨ���ڴ����жϷ���������
} 
void USART2_IRQHandler(void)
{
	u8 res;	
	if(USART2->ISR&(1<<5))//���յ�����
	{	 
		res=USART2->RDR; 
/*		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		} */
	while((USART2->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART2->TDR = (u8) res;      		
	} 
	USART2->ICR|=1<<3;	//����������,������ܻῨ���ڴ����жϷ���������
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//ע��:USART1��ʱ��,�ǿ���ͨ��RCC_D2CCIP2Rѡ���
//��������һ����Ĭ�ϵ�ѡ�񼴿�(Ĭ��ѡ��rcc_pclk2��Ϊ����1ʱ��)
//pclk2��APB2��ʱ��Ƶ��,һ��Ϊ100Mhz
//bound:������ 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;	   
	temp=(pclk2*1000000)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	RCC->IOPENR|=1<<0;   	//ʹ��PORTB��ʱ��  
	RCC->APBENR2|=1<<14;  	//ʹ�ܴ���1ʱ�� 
	RCC->CCIPR|=1<<0;
	SYSCFG->CFGR1|=3<<3;//PA11,PA12->PA9,PA10
	GPIO_Set_withAF(GPIOA,PIN10|PIN9,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU,0);//PA9,PA10,���ù���,������� 
	//����������
 	USART1->BRR=temp; 		//����������@OVER8=0 	
	USART1->CR1=0;		 	//����CR1�Ĵ���
	USART1->CR1|=0<<28;	 	//����M1=0
	USART1->CR1|=0<<12;	 	//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART1->CR1|=0<<15; 	//����OVER8=0,16�������� 
	USART1->CR1|=1<<3;  	//���ڷ���ʹ�� 
//	USART1->ISR=0x6200e2;
#if EN_USART1_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<2;  	//���ڽ���ʹ��
	USART1->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//��2��������ȼ� 
#endif
	USART1->CR2|=1<<15;
	USART1->CR1|=1<<0;  	//����ʹ��
}
void uart1_sendonebyte(u8 dat)
{
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (u8) dat;      	
}
void uart2_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;	   
	temp=(pclk2*1000000)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	RCC->IOPENR|=1<<0;   	//ʹ��PORTA��ʱ��  
	RCC->APBENR1|=1<<17;  	//ʹ�ܴ���1ʱ�� 
	#if defined(STM32G031xx)||defined(STM32G030xx)
	RCC->CCIPR|=1<<0;	
	#else
	RCC->CCIPR|=1<<2;
	#endif
	GPIO_Set_withAF(GPIOA,PIN2|PIN3,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU,1);//PA9,PA10,���ù���,�������,AF10
	//����������
 	USART2->BRR=temp; 		//����������@OVER8=0 	
	USART2->CR1=0;		 	//����CR1�Ĵ���
	USART2->CR1|=0<<28;	 	//����M1=0
	USART2->CR1|=0<<12;	 	//����M0=0&M1=0,ѡ��8λ�ֳ� 
	USART2->CR1|=0<<15; 	//����OVER8=0,16�������� 
	USART2->CR1|=1<<3;  	//���ڷ���ʹ�� 
//	USART2->ISR=0x6200e2;
#if EN_USART2_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<2;  	//���ڽ���ʹ��
	USART2->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 
#endif
	USART2->CR1|=1<<0;  	//����ʹ��
}
void uart2_sendonebyte(u8 dat)
{
	while((USART2->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART2->TDR = (u8) dat;      	
}













