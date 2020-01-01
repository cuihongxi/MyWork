#ifndef __LED_SHOW_H
#define __LED_SHOW_H
/********************************************************************************
*Explain:       共地双色LED指示灯
*
********************************************************************************/
#include "uhead.h"

#define         LED_RED            	GPIOD,GPIO_Pin_0         //红灯
#define         LED_GREEN           GPIOA,GPIO_Pin_4 		
typedef enum{
	RED,
	GREEN,
}LEDType;


void LED_GPIO_Init();   //双色LED初始化

void LEN_RED_Open();
void LEN_RED_Close();
void LEN_GREEN_Open();
void LEN_GREEN_Close();
void LedSharpInIT(u8* time,bool success,u32 systime,u32 ontime,u32 offtime);// 中断中快闪


#endif
