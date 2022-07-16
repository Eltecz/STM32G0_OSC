/*
 * @Description:
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc_v1_release\user\timer.c
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
��ʱ��ʱ�ӣ�APBENR1,APBENR2
�ٶ�=systick
*/
u8 led_bit;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & 0X0001) //����ж�
	{
		GPIO_Pin_Set(GPIOA, PIN11, led_bit);
		led_bit = led_bit ? 0 : 1;
	}
	TIM3->SR &= ~(1 << 0); //����жϱ�־λ
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	GPIO_Set(GPIOA, PIN11, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PD);
	RCC->APBENR1 |= 1 << 1;			  // TIM3ʱ��ʹ��
	TIM3->ARR = arr - 1;			  //�趨�������Զ���װֵ
	TIM3->PSC = psc - 1;			  //Ԥ��Ƶ��
	TIM3->DIER |= 1 << 0;			  //��������ж�
	TIM3->CR1 |= 1 << 7;			  //ʹ���Զ���װ
	TIM3->CR1 &= ~(3 << 5);			  // CMS[1:0]=00,���ض���ģʽ
	TIM3->CR1 &= ~(1 << 4);			  // DIR=0,���ϼ���ģʽ
	MY_NVIC_Init(1, 3, TIM3_IRQn, 2); //��ռ1�������ȼ�3����2
	TIM3->CR1 |= 0x01;				  //ʹ�ܶ�ʱ��3
	TIM3->EGR |= 1;
	led_bit = 0;
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
// Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM1_adc_Init(u8 t)
{
	RCC->APBENR2 |= 1 << 11;		 // TIM1ʱ��ʹ��
	TIM1->ARR = adc_delay[t][0] - 1; //�趨�������Զ���װֵ
	TIM1->PSC = adc_delay[t][1] - 1; //Ԥ��Ƶ��
	TIM1->CR1 |= 1 << 7;			 //ʹ���Զ���װ
	TIM1->CR2 |= 7 << 20;
	TIM1->CR1 &= ~(3 << 5); // CMS[1:0]=00,���ض���ģʽ
	//	TIM1->CR1&=~(1<<4);				//DIR=0,���ϼ���ģʽ
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
		RCC->APBENR2 |= 1 << 15; // TIM1ʱ��ʹ��
		GPIO_Set_withAF(GPIOA, PIN4, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU, 4);
		TIM14->ARR = (pwm_lut[t][0]) - 1; //�趨�������Զ���װֵ
		TIM14->PSC = (pwm_lut[t][1]) - 1; //Ԥ��Ƶ��
		TIM14->CR1 |= 1 << 7;			  //ʹ���Զ���װ
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
		RCC->APBENR1 |= 1 << 1; // TIM1ʱ��ʹ��
		GPIO_Set_withAF(GPIOB, PIN5, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU, 1);
		TIM3->ARR = (pwm_lut[t][0]) - 1; //�趨�������Զ���װֵ
		TIM3->PSC = (pwm_lut[t][1]) - 1; //Ԥ��Ƶ��
		TIM3->CR1 |= 1 << 7;			  //ʹ���Զ���װ
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
