/********************************************************************************
*Author :       
*Time   :       
*Explain:       
*
********************************************************************************/



#ifndef __FL_H
#define __FL_H

#include "UHEAD.H"
#include "CUI_RTOS.H"

typedef enum{
	BH_Open,
	BH_Close
}BH_dir;


extern	u8		flag_BH;


void BH_FL_GPIO_Init();
void FL_CheckStart();
void FL_CheckStop();
void BH_CheckStart();
void BH_CheckStop();
void FL_Control();


#endif
