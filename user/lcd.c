#include "lcd.h"
#include "lcd_font.h"

u16 fill_color_buf;
#if	defined(__dma_h__) && defined(HARDSPI)



void LCD_Fill(u8 xsta, u8 ysta, u8 xend, u8 yend, u16 color)
{
	u8 i, j,xs,ys;
	u16 pix_num;
	xs=xend-xsta;
	ys=yend-ysta;
	pix_num=xs*ys;
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); 
	fill_color_buf=color;
	SPI1->CR2|=1<<1;	
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1->CR2|=1<<11;		
	SPI1_DMA_START(pix_num);	
#else
	SPI1_DMA_START(pix_num<<1);	
#endif
	while(!(DMA1->ISR&(1<<1)));
	DMA1->IFCR|=1<<0;
	SPI1->CR2-=1<<1;
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1->CR2-=1<<11;			
#endif
}

#else
void LCD_Fill(u8 xsta, u8 ysta, u8 xend, u8 yend, u16 color)
{
	u8 i, j,xs,ys;
	xs=xend-xsta;
	ys=yend-ysta;
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
	for (i = ysta; i < yend; i++)
	{
		for (j = xsta; j < xend; j++)
		{
			LCD_WR_DATA(color);
		}
	}
}
#endif
void LCD_DrawPoint(u8 x, u8 y, u16 color)
{
	LCD_Address_Set(x, y, x, y); 
	LCD_WR_DATA(color);
}

void LCD_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u16 color)
{
	u8 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	char incx, incy, uRow, uCol;
	delta_x = x2 - x1; 
	delta_y = y2 - y1;
	uRow = x1; 
	uCol = y1;
	if (delta_x > 0)
		incx = 1; 
	else if (delta_x == 0)
		incx = 0; 
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; 
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; 
	else
		distance = delta_y;
	for (t = 0; t < distance + 1; t++)
	{
		LCD_DrawPoint(uRow, uCol, color); 
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

void LCD_ShowChar(u8 x, u8 y, u8 num, u16 fc, u16 bc, u8 mode)
{
	u8 temp, t, m, i; 
	u8 x0 = x;
	m = 0;
	num = num - ' ';				
	LCD_Address_Set(x, y, x + 5, y + 11);

	for (i = 0; i < 12; i++)
	{
		temp = ascii_1206[num][i];
		for (t = 0; t < 8; t++)
		{
			if (!mode)
			{
				if (temp & (0x01 << t))
					LCD_WR_DATA(fc);
				else
					LCD_WR_DATA(bc);
				m++;
				if (m % 6 == 0)
				{
					m = 0;
					break;
				}
			}
			else
			{
				if (temp & (0x01 << t))
					LCD_DrawPoint(x, y, fc);
				x++;
				if ((x - x0) == 6)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
}

void LCD_ShowString(u8 x, u8 y, const u8 *p, u16 fc, u16 bc, u8 mode)
{
	while (*p != '\0')
	{
		LCD_ShowChar(x, y, *p, fc, bc, mode);
		x += 6;
		p++;
	}
}
void LCD_ShowIntNum(u8 x, u8 y, u16 num, u8 len, u16 fc, u16 bc)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + t * 6, y, ' ', fc, bc, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + t * 6, y, temp + 48, fc, bc, 0);
	}
}

void LCD_ShowFloatNum1(u8 x, u8 y, float num, u8 len, u16 fc, u16 bc)
{
	u8 t, temp;
	u16 num1;
	num1 = num * 100;
	for (t = 0; t < len; t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			LCD_ShowChar(x + (len - 2) * 6, y, '.', fc, bc, 0);
			t++;
			len += 1;
		}
		LCD_ShowChar(x + t * 6, y, temp + 48, fc, bc, 0);
	}
}

void LCD_Clear(u16 color)
{

	LCD_Address_Set(0, 0, LCD_W - 1, LCD_H - 1);
	fill_color_buf=color;
	SPI1->CR2|=1<<1;	
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1->CR2|=1<<11;
	SPI1_DMA_START(LCD_PIX_NUM);	
#else
	SPI1_DMA_START(LCD_PIX_NUM<<1);	
#endif
	while(!(DMA1->ISR&(1<<1)));
	DMA1->IFCR|=1<<0;
	SPI1->CR2-=1<<1;
#ifdef	AUTO_CHANGE_SPI_LENGTH
	SPI1->CR2-=1<<11;			
#endif
}
void LCD_Test_u(float t)
{
	LCD_ShowString(10,20,"LCD_W:",RED,WHITE,0);
	LCD_ShowIntNum(58,20,LCD_W,3,RED,WHITE);
	LCD_ShowString(10,40,"LCD_H:",RED,WHITE,0);
	LCD_ShowIntNum(58,40,LCD_H,3,RED,WHITE);
	LCD_ShowFloatNum1(10,60,t,4,RED,WHITE);
}

/*****************************************************************************
 * @name       :void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
 * @date       :2018-08-09 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
								x2:the ending x coordinate of the rectangle
								y2:the ending y coordinate of the rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}  