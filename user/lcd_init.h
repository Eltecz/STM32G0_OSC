/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1\user\lcd_init.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 21:15:17
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-06-12 21:31:16
 */
#ifndef __lcd_init_h__
#define __lcd_init_h__

#include "per_conf.h"

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏




#ifdef ST7735_160_80
    #define OLD_SCREEN //中景园的屏幕由于批次原因，部分屏幕的时序不同，当驱动屏幕出错时可修改此参数来修正屏幕显示出错

    #if USE_HORIZONTAL==0||USE_HORIZONTAL==1
    #define LCD_W 80
    #define LCD_H 160
    #else
    #define LCD_W 160
    #define LCD_H 80
    #endif

    #ifdef  OLD_SCREEN
    #define fix_x 1
    #define fix_y 2
    #endif

#endif
#ifdef  ST7789_240_135
    #if USE_HORIZONTAL==0||USE_HORIZONTAL==1
    #define LCD_W 135
    #define LCD_H 240
    #else
    #define LCD_W 240
    #define LCD_H 135
    #endif

#endif
#define LCD_PIX_NUM (LCD_W*LCD_H)

#define HARDSPI 
#define DEV_BORAD 0
#ifdef PINS
// sbit LCD_SCL=P3^2;//SCLK
// sbit LCD_SDA=P3^4;//MOSI
// sbit LCD_RES=P3^6;//RES
// sbit LCD_DC =P3^3;//DC
// sbit LCD_CS =P3^5; //CS
// sbit LCD_BLK=P3^5; //BLK


#else  
// sbit LCD_SCL=P1^5;//SCLK
// sbit LCD_SDA=P1^3;//MOSI
// sbit LCD_DC =P1^4;//DC
// sbit LCD_CS =P1^2; //CS
#define LCD_SCL(x) (x?((GPIOA->BSRR)|=(1<<1)):((GPIOA->BSRR)|=(1<<17)))
#define LCD_SDA(x) (x?((GPIOA->BSRR)|=(1<<2)):((GPIOA->BSRR)|=(1<<18)))
#define LCD_DC(x) (x?((GPIOA->BSRR)|=(1<<7)):((GPIOA->BSRR)|=(1<<23)))
#define LCD_CS(x) (x?((GPIOA->BSRR)|=(1<<5)):((GPIOA->BSRR)|=(1<<21)))
#endif
//-----------------LCD端口定义----------------

#define LCD_SCLK_Clr() LCD_SCL(0)//SCL=SCLK
#define LCD_SCLK_Set() LCD_SCL(1)

#define LCD_MOSI_Clr() LCD_SDA(0)//SDA=MOSI
#define LCD_MOSI_Set() LCD_SDA(1)

#define LCD_DC_Clr() ((GPIOA->BSRR)|=(1<<22))//DC
#define LCD_DC_Set() ((GPIOA->BSRR)|=(1<<6))

#define LCD_RST_Clr()  ((GPIOA->BSRR)|=(1<<22))//CS
#define LCD_RST_Set()  ((GPIOA->BSRR)|=(1<<6))


#define LCD_CS_Clr()  ((GPIOA->BSRR)|=(1<<21))//CS
#define LCD_CS_Set()  ((GPIOA->BSRR)|=(1<<5))



void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u8 x1,u8 y1,u8 x2,u8 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化

#endif