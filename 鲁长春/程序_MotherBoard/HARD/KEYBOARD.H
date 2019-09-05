/*
*矩阵键盘扫描程序
*/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H
#include "UHEAD.H"

//------------------------------------按键扫描----------------------------	

#define KEY_VAL_DER_Z			1
#define KEY_VAL_DER_Y			2
#define KEY_VAL_AM				4
#define KEY_VAL_Y30            	8      
#define KEY_VAL_NULL            0       //没按下

//引脚定义      
#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// <z 触摸板，应该进行上升沿中断检测
#define		GPIO_DER_Y			GPIOB,GPIO_Pin_1		// >Y触摸板，应该进行上升沿中断检测
#define		GPIO_AM				GPIOB,GPIO_Pin_2 		// AM 触摸板，应该进行上升沿中断检测
#define		GPIO_Y30			GPIOB,GPIO_Pin_3 		// Y30 触摸板，应该进行上升沿中断检测


#define     KEY_GPIOMODE        GPIO_Mode_In_PU_IT		

typedef enum{
	off,		//关
	on,			//开
	two,		//双击
	three,		//三连
	four,
	five,
	six,
}keyEnum;

typedef struct{
	GPIO_TypeDef* GPIOx;
	GPIO_Pin_TypeDef GPIO_Pin;
	u32 counter;
	keyEnum val;
	
}keyStr;


extern  u8      flag_exti;
extern  u8 		key_val;

extern  keyStr key_AM ;

void Key_GPIO_Init();
void Key_ScanLeave();                   //松手程序
void Y30_function();


#endif









