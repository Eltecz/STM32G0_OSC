/*
 * @Description:
 * @FilePath: \MDK-ARMd:\¹¤³Ì\STM32\32\g0\osc_v1\user\osc.c
 * @Version: 2.0
 * @Autor: Eltecz
 * @Date: 2022-06-21 15:42:04
 * @LastEditors: Eltecz
 * @LastEditTime: 2022-07-14 10:25:36
 */
#include "osc.h"

#define key_seletion_color	RED



extern u16 adc_delay[][2];
extern u16 pwm_lut[][2];

u8 waveform_db[2][adc_length];
#define sizewdb sizeof(waveform_db[0])
static u8 *wdb1_p = waveform_db[1];
static u8 *wdb2_p = waveform_db[2];
u16 runtime;
u8 un_trig=0;
__ALIGNED(4) static osc_dev oscdev = {
	12, 12, adc_length, 96,
	0, 
	1, (1 << 11), 5, 
	0,0,0,
	12,3.3,0,
	0,0,0,
	0,
	6,7,6,7,
	0,0,
	};

static u8 key_seletion;
static u8 old_seletion;

enum{
	ADC_CHSEL=1,
	ADC_TDIV,
	OSC_HDIV,
	OSC_TWAY,
	OSC_TVOTE,
	OSC_PWM1_FREQ,
	OSC_PWM1_DUTY,
	OSC_PWM2_FREQ,
	OSC_PWM2_DUTY,
};
#define func_kval(x)	(((x+10)==key_seletion)?1:0)

u32 d_abs(u32 a,u32 b,u32 c){
	if(c<a)return a;
	else if(c>b) return b;
	else return c;
}

void LCD_DrawLine_fast(u16 x1, u16 y1, u16 y2, u32 color)
{
	u16 t;
	if (y1 < y2)
	{
		for (t = y1; t < y2 + 1; t++)
			LCD_DrawPoint(x1, t, color);
	}
	else
	{
		for (t = y2; t < y1 + 1; t++)
			LCD_DrawPoint(x1, t, color);
	}
}
void Gui_linemap(u8 *db, u16 x, u16 y, u16 h, u16 l, u16 color)
{
	u8 i;
	for (i = 0; i < l - 1; i++)
	{
		LCD_DrawLine(i + x, *(db + i), i + x, *(db + i + 1), color);
	}
}
void Gui_duallinemap(u8 *db, u8 *db_old, u8 x, u8 y, u8 h, u8 l, u16 color)
{
	u8 i;
	u8 trig_p=d_abs(0,h,h-(oscdev.trig32*h>>oscdev.res));
	for (i = 1; i < l - 1; i++)
	{
		LCD_DrawLine_fast(i + x, *(db_old + i), *(db_old + i + 1), BLACK);
		LCD_DrawLine_fast(i + x, *(db + i), *(db + i + 1), color);
	}
	if(!oscdev.hsize)LCD_DrawLine(x,y+trig_p,x+l,y+trig_p,BROWN);
}

void wave_gen(u8 *db, u8 y, u8 h, u8 l, u16 mw)
{
	u8 i;
	u16 *adcbuf_p = adc_test_buf + mw - adc_length_d2 + 1;
	u32 vote_sum=0;
	u8 resize_v=(oscdev.hsize==0)?1:((oscdev.hsize==1)?2:(oscdev.hsize==2)?5:10);
	for (i = 0; i < l; i++)
	{
		db[i] = h-2-d_abs(0,h,(((*(adcbuf_p + i)) * h *resize_v >> 12)))+ y;
		vote_sum+=(*(adcbuf_p+i));
		if(*(adcbuf_p+i)>oscdev.vote_max)	oscdev.vote_max=*(adcbuf_p+i);
		if(*(adcbuf_p+i)<oscdev.vote_min)	oscdev.vote_min=*(adcbuf_p+i);		
	}
	oscdev.vote_avg=vote_sum/l;
}

void osc_init()
{
	
	runtime = 0;
	key_seletion=1;
	old_seletion=1;
	memset(wdb1_p, 0x00, sizewdb);
	memset(wdb2_p, 0x00, sizewdb);
}

void osc_reload()
{
	oscdev.mw = 0;
	oscdev.freq = 0;
	un_trig=0;
	oscdev.trig32=(oscdev.trig_level*(1<<oscdev.res))/10;
	oscdev.vote_max=oscdev.vote_avg=0;
	oscdev.vote_min=1<<oscdev.res;
	oscdev.tb=adc_delay[oscdev.tdiv][0]*adc_delay[oscdev.tdiv][1]*10;
	if(key_seletion%10>5){
		if(oscdev.pwm1_freq)oscdev.pwm1_tb=MAIN_FOSC*1000000/(pwm_lut[oscdev.pwm1_freq][0]*pwm_lut[oscdev.pwm1_freq][1]);
		if(oscdev.pwm2_freq)oscdev.pwm2_tb=MAIN_FOSC*1000000/(pwm_lut[oscdev.pwm2_freq][0]*pwm_lut[oscdev.pwm2_freq][1]);		
	}

	old_seletion=key_seletion;
}

void p_exchange()
{
	if(runtime%2){
		wdb1_p=waveform_db[0];
		wdb2_p=waveform_db[1];
	}
	else{
		wdb1_p=waveform_db[1];
		wdb2_p=waveform_db[0];
	}
}

void osc_showfreq(u8 x,u8 y){
	char s[10];
	if(oscdev.freq>10000)	sprintf(s,"Freq=%3.1lfKhz",(float)oscdev.freq/1000.0);
	else					sprintf(s,"Freq=%5d hz",oscdev.freq);
	LCD_ShowString(x,y,s,WHITE,BLACK,0);	
}

void osc_showtdiv(u8 x,u8 y){
	char s[10];
//	sprintf(s,"AD%d:",oscdev.adc_ch+1);
//	LCD_ShowString(x,y,s,((key_seletion==ADC_CHSEL))?RED:WHITE,key_seletion_color,!(func_kval(ADC_CHSEL)));	
	if(oscdev.tb<999)	sprintf(s,"%4dns",oscdev.tb);
	else				sprintf(s,"%4dus",oscdev.tb/1000);
	LCD_ShowString(x+24,y,s,((key_seletion==ADC_TDIV))?RED:WHITE,key_seletion_color,!(func_kval(ADC_TDIV)));	
}

void osc_showtrig(u8 x,u8 y){
	u8 p='~'+1+(oscdev.trig_way<<1);
	u16 color_f,color_b;
	char s[20];
	LED_ON();
	if(key_seletion==OSC_TWAY)	color_f=RED;
	else	if((runtime%2)&&(oscdev.trig_way)&&(un_trig)){
		color_f=BLACK;
		LED_OFF();
	}
	else	color_f=WHITE;
	LCD_ShowChar(x,y,p,color_f,key_seletion_color,!(func_kval(OSC_TWAY)));	
	LCD_ShowChar(x+6,y,p+1,color_f,key_seletion_color,!(func_kval(OSC_TWAY)));	
	sprintf(s,"Trig=%1.3lfV",oscdev.tvote*oscdev.trig_level/10);
	LCD_ShowString(x+24,y,s,((key_seletion==OSC_TVOTE)?RED:WHITE),key_seletion_color,!(func_kval(OSC_TVOTE)));
}

void osc_showpwm(u8 x,u8 y){
	char s[10];	
	LCD_ShowString(x,y,"PWM1:      |",WHITE,BLACK,0);	
	if(oscdev.pwm1_freq){
		if(oscdev.pwm1_tb>999999)	sprintf(s,"%3dMhz",oscdev.pwm1_tb/1000000);
		else if(oscdev.pwm1_tb>999)	sprintf(s,"%3dKhz",oscdev.pwm1_tb/1000);
		else	sprintf(s,"%3d hz",oscdev.pwm1_tb);
	}
	else	sprintf(s,"OFF   ");
	LCD_ShowString(x+30,y,s,((key_seletion==OSC_PWM1_FREQ))?RED:WHITE,key_seletion_color,!(func_kval(OSC_PWM1_FREQ)));	
	if(oscdev.pwm1_freq){
		sprintf(s,"%2d%%",oscdev.pwm1_duty*10);
		LCD_ShowString(x+72,y,s,((key_seletion==OSC_PWM1_DUTY))?RED:WHITE,key_seletion_color,!(func_kval(OSC_PWM1_DUTY)));			
	}
	

	LCD_ShowString(x+102,y,"PWM2:      |",WHITE,BLACK,0);	
	if(oscdev.pwm2_freq){
		if(oscdev.pwm2_tb>999999)	sprintf(s,"%3dMhz",oscdev.pwm2_tb/1000000);
		else if(oscdev.pwm2_tb>999)	sprintf(s,"%3dKhz",oscdev.pwm2_tb/1000);
		else	sprintf(s,"%3d hz",oscdev.pwm2_tb);
	}
	else	sprintf(s,"OFF   ");
	LCD_ShowString(x+132,y,s,((key_seletion==OSC_PWM2_FREQ))?RED:WHITE,key_seletion_color,!(func_kval(OSC_PWM2_FREQ)));	
	sprintf(s,"40%%");
	if(oscdev.pwm2_freq){
		sprintf(s,"%2d%%",oscdev.pwm2_duty*10);
		LCD_ShowString(x+174,y,s,((key_seletion==OSC_PWM2_DUTY))?RED:WHITE,key_seletion_color,!(func_kval(OSC_PWM2_DUTY)));	
	}		
}

void draw_fonts()
{
	u8 pos_down1 = oscdev.posy + oscdev.height+1;
	u8 pos_down2=pos_down1+12;
	char s[25];	
	LCD_ShowString(oscdev.posx, 0, "DSO By Eltecz", WHITE, BLACK, 0);
//	osc_showfreq(oscdev.posx,pos_down1);
	if(oscdev.freq>10000)	sprintf(s,"Freq=%5dKhz",oscdev.freq/1000);
	else					sprintf(s,"Freq=%5d hz",oscdev.freq);
	LCD_ShowString(oscdev.posx,pos_down1,s,WHITE,BLACK,0);	
//	osc_showtdiv(oscdev.posx+84,pos_down1);

	sprintf(s,"AD%d:",oscdev.adc_ch+1);
	LCD_ShowString(oscdev.posx+84,pos_down1,s,((key_seletion==ADC_CHSEL))?RED:WHITE,key_seletion_color,!(func_kval(ADC_CHSEL)));	
	if(oscdev.tb<999)	sprintf(s,"%4dns",oscdev.tb);
	else				sprintf(s,"%4dus",oscdev.tb/1000);
	LCD_ShowString(oscdev.posx+108,pos_down1,s,((key_seletion==ADC_TDIV))?RED:WHITE,key_seletion_color,!(func_kval(ADC_TDIV)));	

//	LCD_ShowIntNum(oscdev.posx+80,0,key_seletion,3,GREEN,BLACK);
	osc_showtrig(oscdev.posx+90,0);

	if(key_seletion%10<6){
	sprintf(s,"Max:%1.2lfV Min:%1.2lfV Avg:%1.2lfV Duty:%2d%%",oscdev.vote_max*oscdev.tvote/(1<<oscdev.res),oscdev.vote_min*oscdev.tvote/(1<<oscdev.res),oscdev.vote_avg*oscdev.tvote/(1<<oscdev.res),(oscdev.vote_max-oscdev.vote_min)?d_abs(0,100,oscdev.vote_avg*1000/(oscdev.vote_max-oscdev.vote_min)/10):0);
	LCD_ShowString(oscdev.posx,pos_down2,s,WHITE,BLACK,0);
	}
	else{
		osc_showpwm(oscdev.posx,pos_down2);
	}

	sprintf(s,"%2dX",(oscdev.hsize==0)?1:((oscdev.hsize==1)?2:(oscdev.hsize==2)?5:10));
	LCD_ShowString(oscdev.posx+150,pos_down1,s,((key_seletion==OSC_HDIV))?RED:WHITE,key_seletion_color,!(func_kval(OSC_HDIV)));
	
	
}

void osc_findmw()
{
	u16 i, lens, ff1, ff2;
	ff1 = ff2 = lens = 0;
	i = adc_length_d2;
	if (oscdev.trig_way)
	{
		while ((i < (adc_dma_length - adc_length_d2)))
		{
			if ((((oscdev.trig_way - 1) ? (adc_test_buf[i] >= (oscdev.trig32) && adc_test_buf[i - 1] < (oscdev.trig32)) : ((adc_test_buf[i] <= oscdev.trig32) && adc_test_buf[i - 1] > (oscdev.trig32)))) == 1)
			{

				if ((i > (ff1 + adc_length)))
				{
					break;
				}
				else if (lens)
				{
					ff2 = i;
				}
				else
				{
					oscdev.mw = i;
					ff1 = i;
				}
				lens++;
			}
			i++;
		}
		lens--;
	}
	if (oscdev.mw == 0)
	{
		oscdev.mw = ff1 = adc_length;
		un_trig=1;
	}
	else if ((ff2 == ff1))
	{
		oscdev.freq = MAIN_FOSC*10000000/oscdev.tb* lens  ;

	}
	else
	{
		oscdev.freq = (MAIN_FOSC*10000000/oscdev.tb* lens / (ff2 - ff1)) ;
	}
}


void osc_window()
{
	u8 trig_p=oscdev.posy+d_abs(0,oscdev.height,oscdev.height-(oscdev.trig32*oscdev.height>>oscdev.res));
	osc_findmw();
	Gui_linemap(wdb1_p, oscdev.posx, oscdev.posy, oscdev.height, oscdev.length, BLACK);
	wave_gen(wdb1_p, oscdev.posy + 1, oscdev.height, oscdev.length, oscdev.mw);
	Gui_linemap(wdb1_p, oscdev.posx, oscdev.posy, oscdev.height, oscdev.length, GREEN);
	LCD_DrawRectangle(oscdev.posx, oscdev.posy, oscdev.posx + oscdev.length, oscdev.posy + oscdev.height, BLUE);
	draw_fonts();
//	p_exchange();
	if(!oscdev.hsize)LCD_DrawLine(oscdev.posx, trig_p,oscdev.posx+oscdev.length,trig_p,BROWN);
}
enum{
	KL=1,KM,KR
};
u8 getkey(){
	if (KL())	return	1;
	if (KR())	return	3;
	if (KM())	return	2;
	return 0;
}

void key_search(){
	u8 ki=getkey();
	if(ki){
		delay_ms(250);
		if(key_seletion<10){
			switch (ki)
			{
			case KL:key_seletion=(key_seletion==9)?1:key_seletion+1;break;
			case KM:key_seletion+=10;break;		
			case KR:key_seletion=(key_seletion==1)?9:key_seletion-1;break;			
			}
		}
		else if(key_seletion>10&&key_seletion<100){
			switch (key_seletion-10)
			{
			case ADC_TDIV:
				switch (ki)
				{
				case KL:oscdev.tdiv=(oscdev.tdiv==9)?0:oscdev.tdiv+1;break;
				case KM:key_seletion=ADC_TDIV;break;		
				case KR:oscdev.tdiv=(oscdev.tdiv==0)?9:oscdev.tdiv-1;break;	
				}
				break;
			case OSC_TWAY:
				switch (ki)
				{
				case KL:oscdev.trig_way=(oscdev.trig_way==2)?0:oscdev.trig_way+1;break;
				case KM:key_seletion=OSC_TWAY;break;		
				case KR:oscdev.trig_way=(oscdev.trig_way==0)?2:oscdev.trig_way-1;break;	
				}
				break;
			case OSC_TVOTE:
				switch (ki)
				{
				case KL:oscdev.trig_level=(oscdev.trig_level==9)?1:oscdev.trig_level+1;break;
				case KM:key_seletion=OSC_TVOTE;break;		
				case KR:oscdev.trig_level=(oscdev.trig_level==1)?9:oscdev.trig_level-1;break;
				}
				break;
			case OSC_HDIV:
				switch (ki)
				{
				case KL:oscdev.hsize=(oscdev.hsize==3)?0:oscdev.hsize+1;break;
				case KM:key_seletion=OSC_HDIV;break;		
				case KR:oscdev.hsize=(oscdev.hsize==0)?3:oscdev.hsize-1;break;	
				}	
				break;	
			case ADC_CHSEL:	
				switch (ki)
				{
				case KL:
				case KR:oscdev.adc_ch=oscdev.adc_ch?0:1;break;
				case KM:key_seletion=ADC_CHSEL;break;			
				}	
				break;
			case OSC_PWM1_FREQ:
				switch (ki)
				{
				case KL:oscdev.pwm1_freq=(oscdev.pwm1_freq==15)?0:oscdev.pwm1_freq+1;break;
				case KM:key_seletion=OSC_PWM1_FREQ;break;		
				case KR:oscdev.pwm1_freq=(oscdev.pwm1_freq==0)?15:oscdev.pwm1_freq-1;break;	
				}	
				break;
			case OSC_PWM1_DUTY:
				switch (ki)
				{
				case KL:oscdev.pwm1_duty=(oscdev.pwm1_duty==10)?0:oscdev.pwm1_duty+1;break;
				case KM:key_seletion=OSC_PWM1_DUTY;break;		
				case KR:oscdev.pwm1_duty=(oscdev.pwm1_duty==0)?10:oscdev.pwm1_duty-1;break;	
				}	
				break;	
			case OSC_PWM2_FREQ:
				switch (ki)
				{
				case KL:oscdev.pwm2_freq=(oscdev.pwm2_freq==15)?0:oscdev.pwm2_freq+1;break;
				case KM:key_seletion=OSC_PWM2_FREQ;break;		
				case KR:oscdev.pwm2_freq=(oscdev.pwm2_freq==0)?15:oscdev.pwm2_freq-1;break;	
				}	
				break;
			case OSC_PWM2_DUTY:
				switch (ki)
				{
				case KL:oscdev.pwm2_duty=(oscdev.pwm2_duty==10)?0:oscdev.pwm2_duty+1;break;
				case KM:key_seletion=OSC_PWM2_DUTY;break;		
				case KR:oscdev.pwm2_duty=(oscdev.pwm2_duty==0)?10:oscdev.pwm2_duty-1;break;	
				}	
				break;	
			}
		}
	}

}

void osc_app()
{
	runtime++;
	if(key_seletion!=old_seletion){
		Adc_DMA_Init(oscdev.tdiv,oscdev.adc_ch);
		LCD_Clear(BLACK);
		PWM0_Init(oscdev.pwm1_freq,oscdev.pwm1_duty);
		PWM1_Init(oscdev.pwm2_freq,oscdev.pwm2_duty);
	}
	osc_reload();
	//	for(i=0;i<1024;i++)	adc_test_buf[i]=((i+runtime)<<8)%4096;
	ADC_DMA_START(adc_dma_length);
	osc_window();
	key_search();
}