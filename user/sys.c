#include  "sys.h"
//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量		 
#define use_osc 0
#define unused 1
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
#if unused 
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//设置响应优先级和抢断优先级   	    	  				   
} 
//外部中断配置函数,只针对GPIOA~GPIOK 
//参数:
//GPIOx:0~10,代表GPIOA~GPIOK
//BITx:0~15,代表IO引脚编号.
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APBENR2|=1<<0;								//SYSCFGEN=1,使能SYSCFG时钟  
	EXTI->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);	//清除原来设置！！！
	EXTI->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;		//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR1|=1<<BITx;				//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM&0x01)EXTI->FTSR1|=1<<BITx;	//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR1|=1<<BITx;	//line BITx上事件上升降沿触发
} 	
//清除中断标志位
void NVIC_Clear(u8 BITx)
{
	EXTI->FPR1|=(1<<BITx);
	EXTI->RPR1|=(1<<BITx);
}
#endif
//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率,也是PLL1的p分频输出时钟频率
//Fq:PLL1的q分频输出时钟频率
//Fs:PLL输入时钟频率,可以是HSI,CSI,HSE等. 

//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:1~128.

//CPU频率(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMC时钟频率=((25/25)*440/2)=110Mhz

//外部晶振为25M的时候,推荐值:plln=160,pllm=5,pllp=2,pllq=4.
//得到:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//返回值:0,成功;1,失败。
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllr)
{ 
	u8 status;
//	PWR->CR3&=~(1<<2);				//SCUEN=0,锁定LDOEN和BYPASS位的设置
	RCC->CR&=~(1<<24);				//PLL1ON=0,关闭PLL1
	while((RCC->CR&(1<<25)));//PLL1RDY=1?,等待PLL1清零  	
	RCC->PLLCFGR|=(plln)<<8;
	RCC->PLLCFGR|=(pllm-1)<<4;
	RCC->PLLCFGR|=(pllp-1)<<17;
//	RCC->PLLCFGR|=(pllq-1)<<25;
	RCC->PLLCFGR|=(pllr-1)<<29;
//	RCC->PLLCFGR|=1<<16;	
//	RCC->PLLCFGR|=1<<24;
	RCC->PLLCFGR|=1<<28;
	

	
	RCC->PLLCFGR&=~(3<<0);		//清零
	#if (use_osc)

		status=1;
		RCC->PLLCFGR|=3<<0;		//HSE无法就绪

	#else   

		RCC->PLLCFGR|=2<<0;
		
	#endif
	RCC->CR|=1<<24;				//PLL1ON=1,使能PLL1
	while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,等待PLL1准备好  	
	
	FLASH->ACR|=2<<0;			//LATENCY[2:0]=2,2个CPU等待周期(@VOS1 Level,maxclock=210Mhz)
	FLASH->ACR|=1<<8;
	FLASH->ACR|=1<<9;	
	
	RCC->CFGR&=~(7<<0);
	RCC->CFGR|=2<<0;

	while((RCC->CFGR&(2<<0))==0);
	
	

 
	return status;
}  
//系统时钟初始化函数
//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:2~128.
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllr)
{  
	
	RCC->CR=0x00000100;				//设置HISON,开启内部高速RC振荡，其他位全清零
	RCC->CFGR=0x00000000;			//CFGR清零 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR清零 
	RCC->CIER=0x00000000;			//CIER清零,禁止所有RCC相关中断  
	Sys_Clock_Set(plln,pllm,pllp,pllr);//设置时钟  

	//配置向量表				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(1<<29,0x0);
#else   
	MY_NVIC_SetVectorTable(0,0x0);
#endif 

	FLASH->ACR|=3<<8;//启动指令预存

	RCC->IOPENR|=3;
}		  
//GPIO复用设置
//GPIOx:GPIOA~GPIOK.
//BITx:0~15,代表IO引脚编号.
//AFx:0~7,代表AF0~AF7.
//AF0~7设置情况
//ds12991 STM32G030F6.pdf rev3:table13~table17
//AF0:SPI1/2/SWD/I2S1/2/MCO/TIM14/USART1/IR-OUT/EVENTOUT;   		AF1:USART1/2/TIM3/1/SPI2/IR-OUT;		AF2:TIM 14/16/17;	AF3:NAN;
//AF4:TIM14/USART1/SPI2;	AF5:TIM16/17;         		AF6:I2C1/2; 	AF7:EVENTOUT;
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
} 
//GPIO通用设置 
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速. 
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=MODE<<(pinpos*2);	//设置新的模式 
			if((MODE==0X01)||(MODE==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
		}
	}
} 
//GPIO复用设置pro
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速. 
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//AF0:SPI1/2/SWD/I2S1/2/MCO/TIM14/USART1/IR-OUT/EVENTOUT;   		AF1:USART1/2/TIM3/1/SPI2/IR-OUT;		AF2:TIM 14/16/17;	AF3:NAN;
//AF4:TIM14/USART1/SPI2;	AF5:TIM16/17;         		AF6:I2C1/2; 	AF7:EVENTOUT;
void GPIO_Set_withAF(GPIO_TypeDef* GPIOx,u32 BITx,u32 OTYPE,u32 OSPEED,u32 PUPD,u8 AFx)
{
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=2<<(pinpos*2);	//设置新的模式 
			GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
			GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
			GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
			GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
			GPIOx->AFR[pinpos>>3]&=~(0X0F<<((pinpos&0X07)*4));
			GPIOx->AFR[pinpos>>3]|=(u32)AFx<<((pinpos&0X07)*4);	
					
		}
	}	
	
}
//设置GPIO某个引脚的输出状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//status:引脚状态(仅最低位有效),0,输出低电平;1,输出高电平
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	if(status)GPIOx->BSRR|=pinx;	//设置GPIOx的pinx为1
	else GPIOx->BSRR|=(pinx<<16);				//设置GPIOx的pinx为0
}
//读取GPIO某个引脚的状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//返回值:引脚状态,0,引脚低电平;1,引脚高电平
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if(GPIOx->IDR&pinx)return 1;		//pinx的状态为1
	else return 0;						//pinx的状态为0
}