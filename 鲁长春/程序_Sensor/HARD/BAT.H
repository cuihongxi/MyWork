/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       
*
********************************************************************************/



#ifndef __BAT_H
#define __BAT_H

#include "UHEAD.H"
#include "stm8l15x_adc.h"
#include "ADC_CHECK.H"
#include "CUI_RTOS.H"

#define		BAT_STATE_RED				1
#define		BAT_STATE_NODO				2
#define		BAT_STATE_REDSHARP3			3
#define		BAT_STATE_REDSHARP5			4


typedef struct{
	float val;
	u32 threshold;			// 检测间隔
	u8 flag;				// 每次唤醒。检测BAT电量一次
	u8 state;				// 状态
}BATStr;


extern u8 flag_checkBat;



//电源管理
void BatControl(BATStr* bat,TaskLinkStr* tasklink,TaskStr* taskBatControl);
void ReSetLedCheck();
uint16_t Get_BAT_ADC_Dat(hardChannel hard_channel);
#endif
