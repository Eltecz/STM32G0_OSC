/*
 * @Description: 
 * @FilePath: \MDK-ARMd:\工程\STM32\32\g0\osc_v1\user\lcd.h
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-05-21 21:15:41
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-06-21 15:54:11
 */
#ifndef __lcd_h__
#define __lcd_h__

#include "per_conf.h"

#ifdef __dma_h__
extern  u16 fill_color_buf;
#endif

void LCD_Fill(u8 xsta, u8 ysta, u8 xend, u8 yend, u16 color); //指定区域填充颜色
void LCD_DrawPoint(u8 x, u8 y, u16 color);                    //在指定位置画一个点
void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u16 color);     //在指定位置画一条线

void LCD_ShowChar(u8 x, u8 y, u8 num, u16 fc, u16 bc, u8 mode);
void LCD_ShowString(u8 x, u8 y, const u8 *p, u16 fc, u16 bc, u8 mode); //显示字符串
void LCD_ShowIntNum(u8 x, u8 y, u16 num, u8 len, u16 fc, u16 bc);      //显示整数变量
void LCD_ShowFloatNum1(u8 x, u8 y, float num, u8 len, u16 fc, u16 bc); //显示两位小数变量
void LCD_Clear(u16 color);
void LCD_Test_u(float t);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
u32 mypow(u8 m, u8 n); //求幂
//void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

//画笔颜色
#ifdef OLD_SCREEN
#define WHITE (0xffff - 0xFFFF)
#define BLACK (0xffff - 0x0000)
#define BLUE (0xffff - 0x001F)
#define BRED (0xffff - 0xF81F)
#define GRED (0xffff - 0xFFE0)
#define GBLUE (0xffff - 0x07FF)
#define RED (0xffff - 0xF800)
#define MAGENTA (0xffff - 0xF81F)
#define GREEN (0xffff - 0x07E0)
#define CYAN (0xffff - 0x7FFF)
#define YELLOW (0xffff - 0xFFE0)
#define BROWN (0xffff - 0xBC40)      //棕色
#define BRRED (0xffff - 0xFC07)      //棕红色
#define GRAY (0xffff - 0x8430)       //灰色
#define DARKBLUE (0xffff - 0x01CF)   //深蓝色
#define LIGHTBLUE (0xffff - 0x7D7C)  //浅蓝色
#define GRAYBLUE (0xffff - 0x5458)   //灰蓝色
#define LIGHTGREEN (0xffff - 0x841F) //浅绿色
#define LGRAY (0xffff - 0xC618)      //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE (0xffff - 0xA651)  //浅灰蓝色(中间层颜色)
#define LBBLUE (0xffff - 0x2B12)     //浅棕蓝色(选择条目的反色)
#else
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0xF81F
#define GRED 0xFFE0
#define GBLUE 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0xBC40      //棕色
#define BRRED 0xFC07      //棕红色
#define GRAY 0x8430       //灰色
#define DARKBLUE 0x01CF   //深蓝色
#define LIGHTBLUE 0x7D7C  //浅蓝色
#define GRAYBLUE 0x5458   //灰蓝色
#define LIGHTGREEN 0x841F //浅绿色
#define LGRAY 0xC618      //浅灰色(PANNEL,窗体背景色
#define LGRAYBLUE 0xA651  //浅灰蓝色(中间层颜色
#define LBBLUE 0x2B12     //浅棕蓝色(选择条目的反色
#endif

#endif
