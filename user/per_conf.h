/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\per_conf.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 17:11:57
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-13 21:21:40
 */
#ifndef	__per_conf_h__
#define	__per_conf_h__

#define UN_UART
#define UN_ADC
#define UN_TIMER
#define UN_SPI
#define UN_LCD
#define UN_DMA

#include "sys.h"
//led,key
#include "gpio.h"
//sys_delay
#include "delay.h"
#include "string.h"
#include "stdio.h"

#define MAIN_FOSC 100
#define MAX_ADC_CLK 64
#ifdef UN_UART
    //uart1,uart2
    #include "uart.h"
#endif

#ifdef UN_ADC
    //adc
    #define adc_length  220
    #define adc_length_d2   (adc_length>>1)
    #define adc_length_25   ((adc_length*3)>>2)
    #define adc_dma_length  (adc_length<<2)     
    #include "osc.h" 
    #include "adc.h"
#endif

#ifdef UN_TIMER
    //timer 3
    #include "timer.h"
#endif

#ifdef UN_SPI
    //spi1,2
    #include "spi.h"
    #ifdef UN_LCD
        #include "lcd.h"
//        #define ST7735_160_80
        #define ST7789_240_135
        #include "lcd_init.h"        
//        #define AUTO_CHANGE_SPI_LENGTH
    #endif    
#endif

#ifdef UN_DMA
    #include "dma.h"
#endif

#endif