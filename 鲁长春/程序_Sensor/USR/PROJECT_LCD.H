/*
*本工程使用到的显示函数
*2017年11月26日11:45:04
*/

#ifndef __PROJECT_LCD_H
#define __PROJECT_LCD_H

#include "UHEAD.H"
#include "LCDcui.h"

//定义显示位置
#define         LCD_HR_POS              6,2      //湿度
#define         LCD_TP_POS              6,5      //温度
#define         LCD_MODE_POS            12,0         
#define         LCD_TIMEON_POS          70,0   
#define         LCD_TIMEOFF_POS         97,0  
#define         LCD_MIN_HR_POS          52,3 
#define         LCD_MAX_HR_POS          90,3
#define         LCD_MIN_TP_POS          52,6 
#define         LCD_MAX_TP_POS          90,6
#define         LCD_SLEEP_POS           120,2
/*函数*/
void LCD_ShowInit();                    //初始化显示界面
void LCD_Show_Data(GUI_FONT* font,u8 posx,u8 posy,s16 dat); //在显示屏上显示数值
//在显示屏上显示数值,带正负
void LCD_Show_ZFData(GUI_FONT* font,u8 posx,u8 posy,s16 dat);


#endif