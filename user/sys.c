#include  "sys.h"
//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����		 
#define use_osc 0
#define unused 1
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
#if unused 
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//������Ӧ���ȼ����������ȼ�   	    	  				   
} 
//�ⲿ�ж����ú���,ֻ���GPIOA~GPIOK 
//����:
//GPIOx:0~10,����GPIOA~GPIOK
//BITx:0~15,����IO���ű��.
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APBENR2|=1<<0;								//SYSCFGEN=1,ʹ��SYSCFGʱ��  
	EXTI->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);	//���ԭ�����ã�����
	EXTI->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;		//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI->IMR1|=1<<BITx;				//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
	if(TRIM&0x01)EXTI->FTSR1|=1<<BITx;	//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR1|=1<<BITx;	//line BITx���¼��������ش���
} 	
//����жϱ�־λ
void NVIC_Clear(u8 BITx)
{
	EXTI->FPR1|=(1<<BITx);
	EXTI->RPR1|=(1<<BITx);
}
#endif
//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��,Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
//Fq:PLL1��q��Ƶ���ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,CSI,HSE��. 

//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:1~128.

//CPUƵ��(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMCʱ��Ƶ��=((25/25)*440/2)=110Mhz

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=160,pllm=5,pllp=2,pllq=4.
//�õ�:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllr)
{ 
	u8 status;
//	PWR->CR3&=~(1<<2);				//SCUEN=0,����LDOEN��BYPASSλ������
	RCC->CR&=~(1<<24);				//PLL1ON=0,�ر�PLL1
	while((RCC->CR&(1<<25)));//PLL1RDY=1?,�ȴ�PLL1����  	
	RCC->PLLCFGR|=(plln)<<8;
	RCC->PLLCFGR|=(pllm-1)<<4;
	RCC->PLLCFGR|=(pllp-1)<<17;
//	RCC->PLLCFGR|=(pllq-1)<<25;
	RCC->PLLCFGR|=(pllr-1)<<29;
//	RCC->PLLCFGR|=1<<16;	
//	RCC->PLLCFGR|=1<<24;
	RCC->PLLCFGR|=1<<28;
	

	
	RCC->PLLCFGR&=~(3<<0);		//����
	#if (use_osc)

		status=1;
		RCC->PLLCFGR|=3<<0;		//HSE�޷�����

	#else   

		RCC->PLLCFGR|=2<<0;
		
	#endif
	RCC->CR|=1<<24;				//PLL1ON=1,ʹ��PLL1
	while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,�ȴ�PLL1׼����  	
	
	FLASH->ACR|=2<<0;			//LATENCY[2:0]=2,2��CPU�ȴ�����(@VOS1 Level,maxclock=210Mhz)
	FLASH->ACR|=1<<8;
	FLASH->ACR|=1<<9;	
	
	RCC->CFGR&=~(7<<0);
	RCC->CFGR|=2<<0;

	while((RCC->CFGR&(2<<0))==0);
	
	

 
	return status;
}  
//ϵͳʱ�ӳ�ʼ������
//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~128.
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllr)
{  
	
	RCC->CR=0x00000100;				//����HISON,�����ڲ�����RC�񵴣�����λȫ����
	RCC->CFGR=0x00000000;			//CFGR���� 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR���� 
	RCC->CIER=0x00000000;			//CIER����,��ֹ����RCC����ж�  
	Sys_Clock_Set(plln,pllm,pllp,pllr);//����ʱ��  

	//����������				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(1<<29,0x0);
#else   
	MY_NVIC_SetVectorTable(0,0x0);
#endif 

	FLASH->ACR|=3<<8;//����ָ��Ԥ��

	RCC->IOPENR|=3;
}		  
//GPIO��������
//GPIOx:GPIOA~GPIOK.
//BITx:0~15,����IO���ű��.
//AFx:0~7,����AF0~AF7.
//AF0~7�������
//ds12991 STM32G030F6.pdf rev3:table13~table17
//AF0:SPI1/2/SWD/I2S1/2/MCO/TIM14/USART1/IR-OUT/EVENTOUT;   		AF1:USART1/2/TIM3/1/SPI2/IR-OUT;		AF2:TIM 14/16/17;	AF3:NAN;
//AF4:TIM14/USART1/SPI2;	AF5:TIM16/17;         		AF6:I2C1/2; 	AF7:EVENTOUT;
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
} 
//GPIOͨ������ 
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
//MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
//OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
//OSPEED:0~3;����ٶ�����,0,����;1,����;2,����;3,����. 
//PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
//ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=BITx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=MODE<<(pinpos*2);	//�����µ�ģʽ 
			if((MODE==0X01)||(MODE==0X02))	//��������ģʽ/���ù���ģʽ
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//�����µ��ٶ�ֵ  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
				GPIOx->OTYPER|=OTYPE<<pinpos;		//�����µ����ģʽ
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//�����µ�������
		}
	}
} 
//GPIO��������pro
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
//MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
//OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
//OSPEED:0~3;����ٶ�����,0,����;1,����;2,����;3,����. 
//PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
//AF0:SPI1/2/SWD/I2S1/2/MCO/TIM14/USART1/IR-OUT/EVENTOUT;   		AF1:USART1/2/TIM3/1/SPI2/IR-OUT;		AF2:TIM 14/16/17;	AF3:NAN;
//AF4:TIM14/USART1/SPI2;	AF5:TIM16/17;         		AF6:I2C1/2; 	AF7:EVENTOUT;
void GPIO_Set_withAF(GPIO_TypeDef* GPIOx,u32 BITx,u32 OTYPE,u32 OSPEED,u32 PUPD,u8 AFx)
{
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=BITx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=2<<(pinpos*2);	//�����µ�ģʽ 
			GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
			GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//�����µ��ٶ�ֵ  
			GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
			GPIOx->OTYPER|=OTYPE<<pinpos;		//�����µ����ģʽ
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//�����µ�������
			GPIOx->AFR[pinpos>>3]&=~(0X0F<<((pinpos&0X07)*4));
			GPIOx->AFR[pinpos>>3]|=(u32)AFx<<((pinpos&0X07)*4);	
					
		}
	}	
	
}
//����GPIOĳ�����ŵ����״̬
//GPIOx:GPIOA~GPIOK.
//pinx:����λ��,��Χ:1<<0 ~ 1<<15
//status:����״̬(�����λ��Ч),0,����͵�ƽ;1,����ߵ�ƽ
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	if(status)GPIOx->BSRR|=pinx;	//����GPIOx��pinxΪ1
	else GPIOx->BSRR|=(pinx<<16);				//����GPIOx��pinxΪ0
}
//��ȡGPIOĳ�����ŵ�״̬
//GPIOx:GPIOA~GPIOK.
//pinx:����λ��,��Χ:1<<0 ~ 1<<15
//����ֵ:����״̬,0,���ŵ͵�ƽ;1,���Ÿߵ�ƽ
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if(GPIOx->IDR&pinx)return 1;		//pinx��״̬Ϊ1
	else return 0;						//pinx��״̬Ϊ0
}