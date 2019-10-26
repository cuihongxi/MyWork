#include "BAT.H"
#include "lowpower.h"
#include "ADC_CHECK.H"
#include "LED_SHOW.H"
#include "MX830Motor.h"
extern	BATStr bat;						// 电池管理
u8 state = 0;

bool BatState()
{
	return (bool)(state != bat.state);
}

uint16_t Get_BAT_ADC_Dat(hardChannel hard_channel)
{
	uint16_t dat = 0;
	GPIO_RESET(BatControl_GPIO);
	dat = Get_ADC_Dat( hard_channel);
	GPIO_SET(BatControl_GPIO);
	return dat;
}
//电源管理
void BatControl(BATStr* bat,TaskLinkStr* tasklink,TaskStr* taskBatControl)
{
	if(bat->flag)
	{
		bat->flag = 0;
		bat->val = BatteryGetAD(Get_BAT_ADC_Dat(Battery_Channel));
		if( GPIO_READ(CHARGE_PRO_PIN) != RESET)		//充电状态
		{
		    	motorStruct.erro |= ERROR_CHARG;
		    	bat->threshold = GetSysTime(taskBatControl->timerlink) + 1000;	//1秒1次
			if(bat->val >= VALVE_BAT_CHARGE)state = BAT_STATE_CHARGE_H;
			else state = BAT_STATE_CHARGE_L;
		}else 	
		{
		    	motorStruct.erro &= ~ERROR_CHARG;
			if(bat->val >=VALVE_BAT_CHECK) 
			    bat->threshold = GetSysTime(taskBatControl->timerlink) + TIM_BAT_CHECKH;	// 计算检测间隔
			else bat->threshold = GetSysTime(taskBatControl->timerlink) + TIM_BAT_CHECKL;
			if(bat->val > VALVE_BAT_GREEN)			
			    state = BAT_STATE_GREEN;									
			else if(bat->val > VALVE_BAT_SHARP3)	
			    state = BAT_STATE_GREENSHARP3;
			else if(bat->val > VALVE_BAT_Motor) 	
			    state = BAT_STATE_REDSHARP3;								
			else if(bat->val > VALVE_BAT_NoBACK)	
			    state = BAT_STATE_38BC1;							
			else					
			{
				state = BAT_STATE_NoBACK;
				motorStruct.erro |= ERROR_BAT;
			}	
		
		}

		if(motorStruct.erro & ERROR_BAT)
		{
			if(bat->val > VALVE_BAT_NoBACK) motorStruct.erro &= ~ERROR_BAT;
		}
	}
	if((bat->state != state) && (taskBatControl->state == Stop)) // taskBatControl->state == Wait ||
	{
		bat->state = state;
		LEN_GREEN_Close();
		LEN_RED_Close();
		OS_AddFunction(taskBatControl,OS_DeleteTask,0);				// 移除任务
		if(state == BAT_STATE_CHARGE_H)
		{
			debug("电池充电充满\r\n");												
			OS_AddFunction(taskBatControl,LEN_RED_Open,IRQ_PERIOD);		
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);	
		}else		
		if(state == BAT_STATE_CHARGE_L)
		{
			debug("电池正在充电\r\n");												
			OS_AddFunction(taskBatControl,LEN_GREEN_Open,IRQ_PERIOD);		
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);	
		}
		else
		if(state == BAT_STATE_GREEN)
		{
			debug("bat->val ＞ 8.4V时绿灯常亮\r\n");												
			OS_AddFunction(taskBatControl,LEN_GREEN_Open,IRQ_PERIOD);		// ＞8.4V时绿灯常亮。
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);	
		}		
		else if(state == BAT_STATE_GREENSHARP3)					// ＞7.4V绿灯闪亮3次
		{
			debug("bat->val ＞ 7.4V绿灯闪亮3次\r\n");
			OS_AddFunction(taskBatControl,LEN_GREEN_Open,200);	
			OS_AddFunction(taskBatControl,LEN_GREEN_Close,200);				
			OS_AddCycleFunction(taskBatControl,3);				//重复3次	
	
		}else if(state == BAT_STATE_REDSHARP3) 	//＜7.4V红灯闪亮3次。后转为每3秒闪亮一次红灯欠压指示。
		{
			debug("bat->val ＜ 7.4V红灯闪亮3次,后转为每3秒闪亮一次红灯欠压指示\r\n");
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,200,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,200,BatState);
			OS_AddCycleFunction(taskBatControl,3);				//重复3次
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,200,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,2800,BatState);
			
		}else if(state == BAT_STATE_38BC1)	// ＜7.2V马达无条件正转至38BC1高电平\
												红色LED每秒1次闪亮报警30秒，后转为每5秒一次欠压报警。
		{
			debug("bat->val ＜ 7.2V 红色LED每秒1次闪亮报警30秒，后转为每5秒一次欠压报警\r\n");
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,200,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,800,BatState);				
			OS_AddCycleFunction(taskBatControl,3);							// 重复30次
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,200,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,4800,BatState);					
		}else								// 当电压＜6.9V时系统不再响应反转信号，\
												红色LED每秒1次闪亮报警30秒。
		{
			debug("bat->val ＜6.9V 红色LED每秒1次闪亮报警30秒\r\n");			
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,200,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,800,BatState);				
			OS_AddCycleFunction(taskBatControl,30);
		}
		OS_AddTask(tasklink,taskBatControl);// 添加电池电量检测任务
	}
}

void ReSetLedCheck()
{
	flag_checkBat = 1;
	debug("ReSetLedCheck\r\n");
}

