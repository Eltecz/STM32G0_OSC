/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\timer.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 15:30:47
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-13 23:59:28
 */
#ifndef __TIMER_H
#define __TIMER_H
#include "per_conf.h"

extern u16 adc_delay[][2];
extern u16 pwm_lut[][2];
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM1_adc_Init(u8 t);
void PWM0_Init(u8 t,u8 d);
void PWM1_Init(u8 t,u8 d);
#endif























