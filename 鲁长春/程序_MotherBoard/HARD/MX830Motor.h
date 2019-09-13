/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       �����������
*		BC1Ϊ�ش���λ��BC2Ϊ������λ
*		��ǰ�ش�����󿪴�
*		Z����ش����ҷ��򿪴�
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
	MOTOR_NULL  = 0,	//û��ָ��
    FORWARD = 1,        //ǰ��        
    BACK    = 2,        //����
    STOP    = 3,        //ֹͣ
    HOLD    = 4,        //ɲ��
}State_Dir;             //��﷽����

typedef struct
{
    u32  		counter;        		// ���������м�ʱ
    State_Dir   dir;          			// ��﷽��
	State_Dir   command;				// ָ��
	u8 			flag_BC;				// BC��λ����־
	u8 			flag_BC1 ;				// BC1�ش���־
	u8 			flag_BC2 ;				// BC2������־
}Motor_Struct;




extern  Motor_Struct    motorStruct;                            //���״̬�ṹ��
extern 	TaskStr* taskMotor;										// ����˶�����

void MX830Motor_GPIOInit(void);                                 //���IO����
void MX830Motor_StateDir(Motor_Struct* motorStruct);            //�����ƺ���

void MotorControl();		//����˶�
void CheckBC1BC2();			// BC1,BC2 


void Motor_STOP();
void Motor_Z();




#endif