/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc\user\dma.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-22 11:28:12
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-22 16:43:06
 */
#ifndef __dma_h__
#define __dma_h__

#include "per_conf.h"

void SPI1_LCD_DMA_INIT();
void SPI1_DMA_START(u32 ndtr);

void ADC_DMA_CHNNAL_INIT();
void ADC_DMA_START(u32 ndtr);
#endif