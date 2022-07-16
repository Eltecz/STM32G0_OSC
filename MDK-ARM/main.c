/*
 * @Description:
 * @FilePath: \MDK-ARM\main.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:46
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-14 19:45:55
 */

#include "sys.h"
#include "per_conf.h"

int main(void)
{
	u8 i;
	u32 t;
	Stm32_Clock_Init(MAIN_FOSC, 4, 4, 4);
	delay_init(MAIN_FOSC);
	en_rcc();
	PWM0_Init(6,7);
	PWM1_Init(6,7);
	Adc_DMA_Init(0,0);
	gpio_init();
	SPI1_Init();
	LCD_Init();
	LCD_Clear(BLACK);
	osc_init();
	while (1)
	{
		osc_app();
	}
	
}