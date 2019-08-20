/**
 *  �͹������� 
 */

#ifndef __lowpower_h
#define __lowpower_h

#include "uhead.h"

#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// >z �����壬Ӧ�ý����������жϼ��
#define		GPIO_AM				GPIOB,GPIO_Pin_2 		// AM �����壬Ӧ�ý����������жϼ��
#define		GPIO_DER_Z			GPIOB,GPIO_Pin_0 		// <z �����壬Ӧ�ý����������жϼ��
#define		GPIO_DER_Y			GPIOB,GPIO_Pin_3		// >Y�����壬Ӧ�ý����������жϼ��	
#define		GPIO_BH				GPIOC,GPIO_Pin_0		// BH�������,�½����ж�,����ﲻ��ʱӦ��ֹͣ�жϼ��,����1
#define		GPIO_FLU			GPIOA,GPIO_Pin_6		//	FL-U�������,�½����ж�
#define		GPIO_38KHZ_OUT		GPIOD,GPIO_Pin_5		// 38khz����ڣ���ʱ�����	 
#define		GPIO_AM_D			GPIOB,GPIO_Pin_6		// ����LED
#define		GPIO_Y30_D			GPIOD,GPIO_Pin_6		// Y30 led


#define		GPIO_HWKZ			GPIOB,GPIO_Pin_4 		// �������
#define		GPIO_38KHZ_BC1		GPIOB,GPIO_Pin_7		// �����ⷵ���źţ��ߵ�ƽ��Ч
#define		GPIO_38KHZ_BC2		GPIOD,GPIO_Pin_4		// �����ⷵ���źţ��ߵ�ƽ��Ч	 
#define		GPIO_DM				GPIOD,GPIO_Pin_7		// �����
#define		GPIO_BAT_CON		GPIOA,GPIO_Pin_0		// ��ص���������
#define		GPIO_SYSLED_G		GPIOB,GPIO_Pin_5		// ϵͳLED ��ɫ
#define		GPIO_SYSLED_R		GPIOD,GPIO_Pin_1		// ϵͳLED ��ɫ
	 





	 
	 
void LowPowerSet(void);
#endif
