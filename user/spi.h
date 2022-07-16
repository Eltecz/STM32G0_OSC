/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc\user\spi.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 17:02:19
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-05-21 21:24:25
 */
#ifndef __spi_h__
#define __spi_h__

#include "per_conf.h"

void SPI1_Init(void);
void SPI1_SetSpeed(u8 SpeedSet);
void SPI1_WriteByte(u8 TxData);

#endif
