/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       �����������
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
    FORWARD = 0,        //ǰ��        
    BACK    = 1,        //����
    STOP    = 2,        //ֹͣ
    HOLD    = 3,        //ɲ��
}State_Dir;             //��﷽����

typedef struct
{
    u8  flag_dirChanged;        //�����﷽���Ƿ�ı�
    State_Dir   dir;            //��﷽��  
}Motor_Struct;


extern  Motor_Struct    motorStruct;                            //���״̬�ṹ��

void MX830Motor_GPIOInit(void);                                 //���IO����
void MX830Motor_StateDir(Motor_Struct* motorStruct);            //�����ƺ���








#endif