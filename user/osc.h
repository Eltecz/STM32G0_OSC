/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\osc.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-06-21 15:42:04
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-13 23:27:58
 */
#ifndef __osc_h__
#define __osc_h__

#include "per_conf.h"


typedef struct 
{
    u8 posx;
    u8 posy;
    u8 length;
    u8 height;

    u16 mw;

    u8 trig_way;
    u16 trig32;
    u8 trig_level;
    
    u32 freq;
    u32 tb;
    u32 tdiv;

    u8 res;
    float tvote;
    u8 hsize;

    u16 vote_max;
    u16 vote_min;
    u16 vote_avg;

    u8 adc_ch;

    u8 pwm1_freq;
    u8 pwm1_duty;

    u8 pwm2_freq;
    u8 pwm2_duty;

    u32 pwm1_tb;
    u32 pwm2_tb;
    
} osc_dev;

void osc_init();


#endif