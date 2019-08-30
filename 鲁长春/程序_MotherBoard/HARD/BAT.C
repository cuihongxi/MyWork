#include "BAT.H"
#include "lowpower.h"
#include "ADC_CHECK.H"
#include "LED_SHOW.H"

extern TaskLinkStr* tasklink;			// 任务列表
float BATdat = 0;						// 电池电量
TaskStr* taskBatControl;		// 电池电量检测任务
u8 flag_checkBat = 1;			// 每次唤醒。检测BAT电量一次

//电源管理
void BatControl()
{
	if(flag_checkBat && (taskBatControl->state == Wait || taskBatControl->state == Stop))
	{
		flag_checkBat = 0;
		BATdat = BatteryGetAD(Get_ADC_Dat(Battery_Channel));
		OS_AddFunction(taskBatControl,OS_DeleteTask,0);						// 移除任务
		if(BATdat > VALVE_BAT_GREEN)
		{
			debug("BATdat ＞ 8.4V时绿灯常亮\r\n");
			LED_GREEN_Open(1);												// ＞8.4V时绿灯常亮。
		}		
		else if(BATdat > VALVE_BAT_SHARP3)									// ＞7.4V绿灯闪亮3次
		{
			debug("BATdat ＞ 7.4V绿灯闪亮3次\r\n");
			OS_LED_Sharp(taskBatControl,GREEN,200,3);
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);		
			OS_AddTask(tasklink,taskBatControl);							// 添加电池电量检测任务

		}else if(BATdat > VALVE_BAT_Motor) 									//＜7.4V红灯闪亮3次。后转为每3秒闪亮一次红灯欠压指示。
		{
			debug("BATdat ＜ 7.4V红灯闪亮3次\r\n");
			OS_LED_Sharp(taskBatControl,RED,200,3);
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);
			OS_AddFunction(taskBatControl,LEN_RED_Open,200);	
			OS_AddFunction(taskBatControl,LEN_RED_Close,3000);	
			OS_AddTask(tasklink,taskBatControl);							// 添加电池电量检测任务
		}else if(BATdat > VALVE_BAT_NoBACK)									// ＜7.2V马达无条件正转至38BC1高电平\
																			红色LED每秒1次闪亮报警30秒，后转为每5秒一次欠压报警。
		{
			debug("BATdat ＜ 7.2V 红色LED每秒1次闪亮报警30秒，后转为每5秒一次欠压报警\r\n");
			OS_AddFunction(taskBatControl,LEN_RED_Open,200);	
			OS_AddFunction(taskBatControl,LEN_RED_Close,800);				
			OS_AddCycleFunction(taskBatControl,30);							//重复30次
			OS_AddFunction(taskBatControl,LEN_RED_Open,200);	
			OS_AddFunction(taskBatControl,LEN_RED_Close,5000);					
			OS_AddTask(tasklink,taskBatControl);							// 添加电池电量检测任务	
		}else													//当电压＜6.9V时系统不再响应反转信号，红色LED每秒1次闪亮报警30秒。
		{
			debug("BATdat ＜6.9V 红色LED每秒1次闪亮报警30秒\r\n");			
			OS_AddFunction(taskBatControl,LEN_RED_Open,200);	
			OS_AddFunction(taskBatControl,LEN_RED_Close,800);				
			OS_AddCycleFunction(taskBatControl,30);
			OS_AddTask(tasklink,taskBatControl);							// 添加电池电量检测任务
		}
	}

}

void ReSetLedCheck()
{
	flag_checkBat = 1;
	debug("ReSetLedCheck\r\n");
}

