/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\����\STM32\32\g0\osc_v1\user\dma.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-22 11:28:06
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-09 10:15:08
 */
#include "dma.h"
/*
SPI1->DMA   DMA_CHNNAL  1
ADC->DMA    DMA_CHNNAL  2
*/
extern u16 fill_color_buf;
extern  u16 adc_test_buf[adc_dma_length];

DMA_Channel_TypeDef *SPI1_LCD_DMA=DMA1_Channel1;
DMAMUX_Channel_TypeDef  *SPI1_LCD_DMA_MUX=DMAMUX1_Channel0;

DMA_Channel_TypeDef *ADC_DMA=DMA1_Channel2;
DMAMUX_Channel_TypeDef  *ADC_DMA_MUX=DMAMUX1_Channel1;

void SPI1_LCD_DMA_INIT()
{

	u8 streamx=1;
 	RCC->AHBENR|=1<<0;//DMA1ʱ��ʹ�� 
	while(SPI1_LCD_DMA->CCR&0X01);//�ȴ�DMA������ 
	DMA1->IFCR|=0X07;//���֮ǰ��stream�ϵ������жϱ�־
	
	SPI1_LCD_DMA->CPAR=(u32)&SPI1->DR;		//DMA�����ַ
	SPI1_LCD_DMA->CMAR=(u32)&fill_color_buf;//DMA �洢��0��ַ
	SPI1_LCD_DMA->CNDTR=0;	//DMA �������ݳ���
	SPI1_LCD_DMA->CCR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	SPI1_LCD_DMA->CCR|=1<<4;		//�洢��������ģʽ
	SPI1_LCD_DMA->CCR|=1<<5;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	SPI1_LCD_DMA->CCR|=0<<6;		//���������ģʽ
	SPI1_LCD_DMA->CCR|=0<<7;		//�洢��������ģʽ
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1_LCD_DMA->CCR|=1<<8;		//�������ݳ���:8λ
#else
	SPI1_LCD_DMA->CCR|=0<<8;		//�������ݳ���:8λ
#endif
	SPI1_LCD_DMA->CCR|=1<<10;		//�洢�����ݳ���:8λ
	SPI1_LCD_DMA->CCR|=1<<12;		//�е����ȼ�
	SPI1_LCD_DMA_MUX->CCR|=(u32)17;//ͨ��ѡ��
	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
}
void SPI1_DMA_START(u32 ndtr)
{
	SPI1_LCD_DMA->CCR&=~(1<<0); 	//�ر�DMA���� 
	while(SPI1_LCD_DMA->CCR&0X1);	//ȷ��DMA���Ա�����  
	SPI1_LCD_DMA->CNDTR=ndtr;		//DMA �������ݳ���
	SPI1_LCD_DMA->CCR|=1<<0;		//����DMA����
}	

void ADC_DMA_CHNNAL_INIT()
{
	RCC->AHBENR|=1<<0;//DMA1ʱ��ʹ�� 
	ADC_DMA->CCR&=~(1<<0); 	//�ر�DMA���� 
	while(ADC_DMA->CCR&0X01);//�ȴ�DMA������ 
	DMA1->IFCR|=0X38;//���֮ǰ��stream�ϵ������жϱ�־
	
	ADC_DMA->CPAR=(u32)&ADC1->DR;		//DMA�����ַ
	ADC_DMA->CMAR=(u32)&adc_test_buf;//DMA �洢��0��ַ
	ADC_DMA->CNDTR=0;	//DMA �������ݳ���
	ADC_DMA->CCR=0;			//��ȫ����λCR�Ĵ���ֵ 
    
//    ADC_DMA->CCR|=1<<1;     //DMA�ж�
	ADC_DMA->CCR|=0<<4;		//���赽�洢��ģʽ
	ADC_DMA->CCR|=1<<5;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	ADC_DMA->CCR|=0<<6;		//���������ģʽ
	ADC_DMA->CCR|=1<<7;		//�洢������ģʽ
	ADC_DMA->CCR|=1<<8;		//�������ݳ���:16λ
	ADC_DMA->CCR|=1<<10;		//�洢�����ݳ���:16λ
	ADC_DMA->CCR|=1<<12;		//�е����ȼ�
	ADC_DMA_MUX->CCR|=(u32)5;//ͨ��ѡ��

}
void ADC_DMA_START(u32 ndtr)
{
	ADC_DMA->CCR&=~(1<<0); 	//�ر�DMA���� 
	while(ADC_DMA->CCR&0X1);	//ȷ��DMA���Ա�����  
	ADC_DMA->CNDTR=ndtr;		//DMA �������ݳ���
	ADC1->CR|=1<<2;       //��������ת��ͨ�� 
	ADC_DMA->CCR|=1<<0;		//����DMA����
	TIM1->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	TIM1->CCER|=1<<12;
	while (!((DMA1->ISR & 0x10)));
	DMA1->IFCR |= 0x10;
	TIM1->CR1-=0x01;    //ʹ�ܶ�ʱ��3
	TIM1->CCER-=1<<12;
}	
//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:0~7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  

void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{ 
	DMA_TypeDef *DMAx=DMA1;
	u8 streamx;
 	RCC->AHBENR|=1<<21;//DMA1ʱ��ʹ�� 
	while(DMA_Streamx->CCR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X8)/0X14;		//�õ�streamͨ����
	DMAx->IFCR|=0X07<<(3*(streamx));//���֮ǰ��stream�ϵ������жϱ�־
	
	DMA_Streamx->CPAR=par;		//DMA�����ַ
	DMA_Streamx->CMAR=mar;		//DMA �洢��0��ַ
	DMA_Streamx->CNDTR=ndtr;	//DMA �������ݳ���
	DMA_Streamx->CCR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CCR|=1<<6;		//�洢��������ģʽ
	DMA_Streamx->CCR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Streamx->CCR|=0<<9;		//���������ģʽ
	DMA_Streamx->CCR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CCR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CCR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CCR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CCR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CCR|=0<<23;		//�洢��ͻ�����δ���
	DMA_Streamx->CCR|=(u32)chx<<25;//ͨ��ѡ��
	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
} 
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CCR&=~(1<<0); 	//�ر�DMA���� 
	while(DMA_Streamx->CCR&0X1);	//ȷ��DMA���Ա�����  
	DMA_Streamx->CNDTR=ndtr;		//DMA �洢��0��ַ 
	DMA_Streamx->CCR|=1<<0;		//����DMA����
}	  
