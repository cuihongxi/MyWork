/********************************************************************************
*Author :       
*Time   :       
*Explain:       
*
********************************************************************************/



#ifndef __FL_H
#define __FL_H

#include "UHEAD.H"


typedef enum{
    BH_Stop,
	BH_Open,
	BH_Close
}BH_dir;

#define		GPIO_BH2		GPIOD,GPIO_Pin_4	
#define		GPIO_BH			GPIOB,GPIO_Pin_7		// BH方波检测,下降沿中断,当马达不动时应该停止中断检测,并置1
#define		GPIO_FLU		GPIOA,GPIO_Pin_6		// FL-U风力检测,下降沿中断


extern	u8		flag_BH;


void BH_FL_GPIO_Init();
void FL_CheckStart();
void FL_CheckStop();
void BH_CheckStart();
void BH_CheckStop();
void FL_Control();


#endif
