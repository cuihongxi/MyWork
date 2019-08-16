/*
*矩阵键盘扫描程序
*/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H
#include "UHEAD.H"

//------------------------------------按键扫描----------------------------	


#define KEY_VAL_ALLOFF          1
#define KEY_VAL_VEG             2
#define KEY_VAL_BLO             3
#define KEY_VAL_ALLON           4
#define KEY_VAL_MOD1            5       //  16/8    
#define KEY_VAL_MOD2            6       //  14/10    
#define KEY_VAL_MOD3            7       //  12/12        
#define KEY_VAL_MOD4            8       //  8/16    
#define KEY_VAL_TIMER           9
#define KEY_VAL_ADD             10
#define KEY_VAL_SUB             11
#define KEY_VAL_NULL            0       //没按下

//引脚定义      
#define         Lie_0                   GPIOB,GPIO_Pin_0
#define         Lie_1                   GPIOB,GPIO_Pin_1
#define         Lie_2                   GPIOB,GPIO_Pin_2
#define         Lie_3                   GPIOB,GPIO_Pin_3
#define         Heng_0                  GPIOB,GPIO_Pin_4
#define         Heng_1                  GPIOB,GPIO_Pin_5
#define         Heng_2                  GPIOB,GPIO_Pin_6


#define         GPIO_MODE_OUT           GPIO_Mode_Out_PP_Low_Slow//GPIO_Mode_Out_OD_Low_Slow//GPIO_Mode_Out_PP_Low_Slow
#define         GPIO_MODE_IN            GPIO_Mode_In_PU_No_IT
#define         GPIO_MODE_IT            GPIO_Mode_In_PU_IT

//#define         KEY_GPIOPORT            GPIOB;

extern  u8      flag_exti  ;
extern  u8      flag_timer ; 

void Key_GPIO_Init();
unsigned char Keyscan(void);		//按键扫描程序 P0口,高5位为输入经上来电阻到电源
void Key_ScanLeave();                   //松手程序

#endif









