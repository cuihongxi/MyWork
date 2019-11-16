#ifndef __LED_SHOW_H
#define __LED_SHOW_H
/********************************************************************************
*Explain:       共地双色LED指示灯
*
********************************************************************************/
#include "uhead.h"

#define         LED_GREEN          GPIOB,GPIO_Pin_4          //绿灯


#ifndef  DM_LED_RESET               
#define         LED_RED            GPIOD,GPIO_Pin_7         //红灯
#else
#define         LED_RED            GPIOD,GPIO_Pin_1         //红灯
#endif



typedef enum{
	RED,
	GREEN,
}LEDType;


void LED_GPIO_Init();   //双色LED初始化
//void LED_GREEN_Open(u8 bol);
//void LED_RED_Open(u8 bol);
void LEN_RED_Open();
void LEN_RED_Close();
void LEN_GREEN_Open();
void LEN_GREEN_Close();
void LedSharpInIT(u8* time,bool success,u32 systime,u32 ontime,u32 offtime);// 中断中快闪
void LEN_REDBEEP_Open();
void LEN_REDBEEP_Close();
void LEN_GREENBEEP_Open();
void LEN_GREENBEEP_Close();

#endif
