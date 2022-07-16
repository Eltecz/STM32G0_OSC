/*
 * @Description:
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1_release\user\timer.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-14 19:48:19
 */
#include "timer.h"
u16 adc_delay[][2] = {
	20,	1,
	25,	2,
	20,	5,
	20,	10,
	20,	25,
	20,	50,
	20,	100,
	20,	250,
	20,500,
	20,1000,
	20,2500,
	20,5000,
};
u16 pwm_lut[][2] = {
	0,	0,
	10,	1,
	10,	2,
	10,	5,
	10,	10,
	10,	20,
	10,	50,
	10,	100,
	10, 200,
	10,	500,
	10,	1000,
	10,	2000,
	10,	5000,
	10,	10000,
	10,	20000,
	10,	50000,
};
/*
定时器时钟：APBENR1,APBENR2
速度=systick
*/
u8 led_bit;
//定时器3中断服务程序
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & 0X0001) //溢出中断
	{
		GPIO_Pin_Set(GPIOA, PIN11, led_bit);
		led_bit = led_bit ? 0 : 1;
	}
	TIM3->SR &= ~(1 << 0); //清除中断标志位
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
// arr：自动重装值。
// psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	GPIO_Set(GPIOA, PIN11, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PD);
	RCC->APBENR1 |= 1 << 1;			  // TIM3时钟使能
	TIM3->ARR = arr - 1;			  //设定计数器自动重装值
	TIM3->PSC = psc - 1;			  //预分频器
	TIM3->DIER |= 1 << 0;			  //允许更新中断
	TIM3->CR1 |= 1 << 7;			  //使能自动重装
	TIM3->CR1 &= ~(3 << 5);			  // CMS[1:0]=00,边沿对齐模式
	TIM3->CR1 &= ~(1 << 4);			  // DIR=0,向上计数模式
	MY_NVIC_Init(1, 3, TIM3_IRQn, 2); //抢占1，子优先级3，组2
	TIM3->CR1 |= 0x01;				  //使能定时器3
	TIM3->EGR |= 1;
	led_bit = 0;
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
// arr：自动重装值。
// psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM1_adc_Init(u8 t)
{
	RCC->APBENR2 |= 1 << 11;		 // TIM1时钟使能
	TIM1->ARR = adc_delay[t][0] - 1; //设定计数器自动重装值
	TIM1->PSC = adc_delay[t][1] - 1; //预分频器
	TIM1->CR1 |= 1 << 7;			 //使能自动重装
	TIM1->CR2 |= 7 << 20;
	TIM1->CR1 &= ~(3 << 5); // CMS[1:0]=00,边沿对齐模式
	//	TIM1->CR1&=~(1<<4);				//DIR=0,向上计数模式
	TIM1->CCMR2 |= 0X6 << 12;
	TIM1->CCMR2 |= 3 << 10;
	TIM1->CCR4 |= (adc_delay[t][0]) >> 1;
	TIM1->CCER |= 1 << 13;
}

void PWM0_Init(u8 t, u8 d)
{
	RCC->APBRSTR2 |= 1 << 15;
	delay_ms(1);
	RCC->APBRSTR2 -= 1 << 15;
	if (t)
	{
		RCC->APBENR2 |= 1 << 15; // TIM1时钟使能
		GPIO_Set_withAF(GPIOA, PIN4, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU, 4);
		TIM14->ARR = (pwm_lut[t][0]) - 1; //设定计数器自动重装值
		TIM14->PSC = (pwm_lut[t][1]) - 1; //预分频器
		TIM14->CR1 |= 1 << 7;			  //使能自动重装
		TIM14->CCMR1 |= 0X6 << 4;
		TIM14->CCMR1 |= 3 << 2;
		TIM14->CCR1 = d;
		TIM14->EGR |= 1 << 0;
		TIM14->CCER |= 1 << 0;
		TIM14->CR1 |= 1 << 0;
	}
	else
		return;
}

void PWM1_Init(u8 t, u8 d)
{
	RCC->APBRSTR1 |= 1 << 1;
	delay_ms(1);
	RCC->APBRSTR1 -= 1 << 1;
	if (t)
	{
		RCC->APBENR1 |= 1 << 1; // TIM1时钟使能
		GPIO_Set_withAF(GPIOB, PIN5, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU, 1);
		TIM3->ARR = (pwm_lut[t][0]) - 1; //设定计数器自动重装值
		TIM3->PSC = (pwm_lut[t][1]) - 1; //预分频器
		TIM3->CR1 |= 1 << 7;			  //使能自动重装
		TIM3->CCMR1 |= 0X6 << 12;
		TIM3->CCMR1 |= 3 << 10;
		TIM3->CCR2 = d;
		TIM3->EGR |= 1 << 0;
		TIM3->CCER |= 1 << 4;
		TIM3->CR1 |= 1 << 0;
	}
	else
		return;
}
