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
#define  MX830Motor_GPIO_BI     GPIOD,GPIO_Pin_5



#define  MX830Motor_GPIOMode    GPIO_Mode_Out_PP_Low_Slow

typedef enum
{
	MOTOR_NULL  	= 0,		//û��ָ��
    	FORWARD 	= 1,        	//ǰ��        
    	BACK    	= 2,        	//����
    	STOP    	= 3,        	//ֹͣ
    	HOLD    	= 4,        	//ɲ��
}State_Dir;             		//��﷽����

typedef struct
{
    	u32  		counter;        // ���������м�ʱ
	u32		opentime ;	// ��¼�����������ʱ��
	u32		closetime ;	// ��¼��������ش�ʱ��
	int		hasrun ;	// ��¼���Ŀǰ��λ�ã��ش�λ��Ϊ0������BC2Ϊ dm_counter
	int 		needrun;	// ��Ҫ�ߵĲ���
    	State_Dir   	dir;          	// ��﷽��
	State_Dir	dirBack;	// ��¼��﷽��ֹͣ����	
	State_Dir   	command;	// ָ��
	u8 		flag_BC;	// BC��λ����־
	u8 		flag_shutdown ;	// �ش���־
	u8 		flag_opendown ;	// ������־
	u8		erro ;		// ������
	
}Motor_Struct;

typedef enum{
	open,
	SHUTDOWN,
	OPENDOWN,
}WindowState;


extern  	Motor_Struct    motorStruct;           	//���״̬�ṹ��
extern 		TaskStr* 	taskMotor;		// ����˶�����
extern 		WindowState	windowstate;
extern 		u8		motor_erro;

void MX830Motor_GPIOInit(void);                         //���IO����
void MX830Motor_StateDir(Motor_Struct* motorStruct);    //�����ƺ���

void MotorControl();					//����˶�
void CheckBC1BC2();					// BC1,BC2 

void Motor_STOP();
void Motor_Z();
WindowState CheckWindowState();
void Motor_RunBack();
#endif