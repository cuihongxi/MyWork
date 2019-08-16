/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       马达驱动函数
*
********************************************************************************/

#ifndef __MX830Motor_H
#define __MX830Motor_H

#include "uhead.h"

#define  MX830Motor_GPIO_FI     GPIOC,GPIO_Pin_2
#define  MX830Motor_GPIO_BI     GPIOC,GPIO_Pin_3
#define  MX830Motor_GPIOMode    GPIO_Mode_Out_PP_Low_Slow

typedef enum
{
    FORWARD = 0,        //前进        
    BACK    = 1,        //后退
    STOP    = 2,        //停止
    HOLD    = 3,        //刹车
}State_Dir;             //马达方向定义

typedef struct
{
    u8  flag_dirChanged;        //标记马达方向是否改变
    State_Dir   dir;            //马达方向  
}Motor_Struct;


extern  Motor_Struct    motorStruct;                            //马达状态结构体

void MX830Motor_GPIOInit(void);                                 //马达IO配置
void MX830Motor_StateDir(Motor_Struct* motorStruct);            //马达控制函数








#endif