/**
 *  �͹������� 
 */

#ifndef __lowpower_h
#define __lowpower_h

#include "uhead.h"



#define		GPIO_BH2		GPIOD,GPIO_Pin_6	
#define		GPIO_BH			GPIOB,GPIO_Pin_6		// BH�������,�½����ж�,����ﲻ��ʱӦ��ֹͣ�жϼ��,����1
#define		GPIO_FLU		GPIOA,GPIO_Pin_6		// FL-U�������,�½����ж�
#define		GPIO_38KHZ_OUT		GPIOD,GPIO_Pin_5		// 38khz����ڣ���ʱ�����	 


#define		GPIO_38KHZ_BC1		GPIOB,GPIO_Pin_7		// �����ⷵ���źţ��͵�ƽ��Ч
#define		GPIO_38KHZ_BC2		GPIOD,GPIO_Pin_4		// �����ⷵ���źţ��͵�ƽ��Ч	 

#define		GPIO_BAT_CON		GPIOA,GPIO_Pin_0		// ��ص���������

	 




	 
	 
void LowPowerSet(void);






#endif


