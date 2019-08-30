/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       
*
********************************************************************************/



#ifndef __BAT_H
#define __BAT_H

#include "UHEAD.H"
#include "CUI_RTOS.H"
#include "stm8l15x_adc.h"

extern u8 flag_checkBat;
extern TaskStr* taskBatControl;



//电源管理
void BatControl();
void ReSetLedCheck();

#endif
