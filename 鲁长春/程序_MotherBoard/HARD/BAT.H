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
#include "ADC_CHECK.H"

#define		BAT_STATE_GREEN				1
#define		BAT_STATE_GREENSHARP3			2
#define		BAT_STATE_REDSHARP3			3
#define		BAT_STATE_38BC1				4
#define		BAT_STATE_NoBACK			5
#define		BAT_STATE_CHARGE_H			6	//充满电
#define		BAT_STATE_CHARGE_L			7	//充电

typedef struct{
	float val;
	u32 threshold;			// 检测间隔
	u8 flag;				// 每次唤醒。检测BAT电量一次
	u8 state;				// 状态
}BATStr;

#define		GPIO_BAT_CON		GPIOA,GPIO_Pin_0		// 电池电量检测控制


extern u8 flag_checkBat;



//电源管理
void BatControl();
void ReSetLedCheck();
uint16_t Get_BAT_ADC_Dat(hardChannel hard_channel);
#endif
