/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       马达驱动函数
*		BC1为关窗限位，BC2为开窗限位
*		向前关窗，向后开窗
*		Z方向关窗，右方向开窗
********************************************************************************/

#ifndef __MX830Motor_H
#define __MX830Motor_H

#include "uhead.h"
#include "CUI_RTOS.H"
#define  MX830Motor_GPIO_FI     GPIOC,GPIO_Pin_2

#if 	DEBUG_LEVEL > 0 
#define  MX830Motor_GPIO_BI     GPIOA,GPIO_Pin_1
#else
#define  MX830Motor_GPIO_BI     GPIOC,GPIO_Pin_3
#endif


#define  MX830Motor_GPIOMode    GPIO_Mode_Out_PP_Low_Slow

typedef enum
{
	MOTOR_NULL  = 0,	//没有指令
    FORWARD = 1,        //前进        
    BACK    = 2,        //后退
    STOP    = 3,        //停止
    HOLD    = 4,        //刹车
}State_Dir;             //马达方向定义

typedef struct
{
    u32  		counter;        		// 标记马达运行计时
	u32			hasrun ;				// 记录马达目前的位置，关窗位置为0，开窗BC2为 dm_counter
	u32			opentime ;				// 记录马达完整开窗时间
	u32			closetime ;				// 记录马达完整关窗时间
    State_Dir   dir;          			// 马达方向
	State_Dir   command;				// 指令
	u8 			flag_BC;				// BC到位检测标志
	u8 			flag_BC1 ;				// BC1关窗标志
	u8 			flag_BC2 ;				// BC2开窗标志
//	u8			led_bc1;				// 用来计算完整开窗时间
//	u8			led_bc2;				// 用来计算完整开窗时间
}Motor_Struct;

typedef enum{
	open,
	to_BC1,
	to_BC2,
}WindowState;


extern  Motor_Struct    motorStruct;                            //马达状态结构体
extern 	TaskStr* taskMotor;										// 马达运动任务
extern 	WindowState				windowstate;


void MX830Motor_GPIOInit(void);                                 //马达IO配置
void MX830Motor_StateDir(Motor_Struct* motorStruct);            //马达控制函数

void MotorControl();		//马达运动
void CheckBC1BC2();			// BC1,BC2 


void Motor_STOP();
void Motor_Z();
void CheckWindowState();



#endif