/*
*矩阵键盘扫描程序
*/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H
#include "UHEAD.H"

//------------------------------------按键扫描----------------------------	


#define KEY_VAL_AM				1
#define KEY_VAL_POW_CA			        2
#define KEY_VAL_Y30 			        3
#define KEY_VAL_I30				4
#define KEY_VAL_I60				5        
#define KEY_VAL_I100            6           
#define KEY_VAL_MOTZ           	7    	//马达左转           
#define KEY_VAL_MOTY            8       //马达右转
#define KEY_VAL_DUIMA           9		//对码
#define KEY_VAL_NULL            0       //没按下

//引脚定义      
#define         Lie_0                   GPIOA,GPIO_Pin_3
#define         Lie_1                   GPIOD,GPIO_Pin_0
#define         Lie_2                   GPIOA,GPIO_Pin_0

#define         Heng_0                  GPIOC,GPIO_Pin_6
#define         Heng_1                  GPIOA,GPIO_Pin_2
#define         Heng_2                  GPIOB,GPIO_Pin_2

#define         Z_LED                   GPIOB,GPIO_Pin_1		// 感应灯
#define         T_LED                   GPIOC,GPIO_Pin_1		// 感应灯

#define		TOUCH_IO		GPIOC,GPIO_Pin_5		// 触摸中断引脚 GPIOC,GPIO_Pin_5
#define		GPIO_MODE_TOUCH		GPIO_Mode_In_FL_IT		

#define         GPIO_MODE_OUT           GPIO_Mode_Out_PP_Low_Slow
#define         GPIO_MODE_IN            GPIO_Mode_In_PU_No_IT
#define         GPIO_MODE_IT            GPIO_Mode_In_PU_IT

//#define         KEY_GPIOPORT            GPIOB;

extern  u8      flag_exti  ;
extern  u8      flag_timer ; 

void Key_GPIO_Init();
unsigned char Keyscan(void);		//按键扫描程序 P0口,高5位为输入经上来电阻到电源
void Key_ScanLeave();                   //松手程序

#endif









