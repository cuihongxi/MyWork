#include "BAT.H"
#include "ADC_CHECK.H"
#include "LED_SHOW.H"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"

extern	BATStr bat;						// 电池管理
extern	Nrf24l01_PTXStr 	ptx ;
extern	u8		CGDAT[5];

u8 state = 0;

void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);//通过NRF向主板发送命令函数

uint16_t Get_BAT_ADC_Dat(hardChannel hard_channel)
{
	uint16_t dat = 0;
	GPIO_RESET(BatControl_GPIO);
	dat = Get_ADC_Dat( hard_channel);
	GPIO_SET(BatControl_GPIO);
	return dat;
}

bool BatState()
{
	return (bool)(state != bat.state);
}

//电源管理
void BatControl(BATStr* bat,TaskLinkStr* tasklink,TaskStr* taskBatControl)
{
  static u8 flag_start = 1;	// 开机首次显示电压标志	
	if(bat->flag)
	{
		bat->flag = 0;
		bat->val = BatteryGetAD(Get_BAT_ADC_Dat(Battery_Channel));	

		if(bat->val >= 3.3) 
			bat->threshold = GetSysTime(taskBatControl->timerlink) + TIM_BAT_6H;	// 计算检测间隔
		else if(bat->val >= 3.0) 
		{
			bat->threshold = GetSysTime(taskBatControl->timerlink) + TIM_BAT_2H;
			NRF_SendCMD(&ptx,CGDAT,CMD_CG_BAT,CMD_CG_BAT);	// 电池电量不足
		}
		else 
		{
			bat->threshold = GetSysTime(taskBatControl->timerlink) + TIM_BAT_0_5H;
			NRF_SendCMD(&ptx,CGDAT,CMD_CG_BAT,CMD_CG_BAT);	// 电池电量不足
		}
		if(flag_start)
		{
			flag_start = 0;
			if(bat->val > 4.2)	
				LEN_RED_Open();								
			else if(bat->val > 3.2)	
			{
				LEN_GREEN_Open();
				delay_ms(100);
				LEN_GREEN_Close();
				delay_ms(100);	
				LEN_GREEN_Open();
				delay_ms(100);
				LEN_GREEN_Close();
				delay_ms(100);	
				LEN_GREEN_Open();
				delay_ms(100);
				LEN_GREEN_Close();
			}
			else 	
			{
				LEN_RED_Open();
				delay_ms(100);
				LEN_RED_Close();
				delay_ms(100);	
				LEN_RED_Open();
				delay_ms(100);
				LEN_RED_Close();
				delay_ms(100);	
				LEN_RED_Open();
				delay_ms(100);
				LEN_RED_Close();			
			}
				
		}else
		{
			if(bat->val > 4.2)	
				state = BAT_STATE_RED;  									
			else if(bat->val > 3.3)	
				state = BAT_STATE_NODO;
			else if(bat->val > 3.0) 	
				state = BAT_STATE_REDSHARP5;								
			else 
				state = BAT_STATE_REDSHARP3;								
		}
	}
	if((bat->state != state) && (taskBatControl->state == Stop))
	{
		bat->state = state;
		LEN_GREEN_Close();
		LEN_RED_Close();
		OS_AddFunction(taskBatControl,OS_DeleteTask,0);				// 移除任务
		if(state == BAT_STATE_RED)
		{
			debug("电池 > 4.2V\r\n");												
			OS_AddFunction(taskBatControl,LEN_RED_Open,IRQ_PERIOD);		
			OS_AddFunction(taskBatControl,OS_DeleteTask,0);	
		}else		
		if(state == BAT_STATE_NODO)
		{
			debug("3.3v <电池 < 4.2V\r\n");												
		}
		else
		if(state == BAT_STATE_REDSHARP5)
		{
			debug("3.0v <电池 < 3.3V\r\n");												
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,500,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,4500,BatState);	// 5秒闪一次

		}		
		else if(state == BAT_STATE_REDSHARP3)					
		{
			debug("电池 < 3.0V\r\n");
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Open,500,BatState);	
			OS_AddJudegeFunction(taskBatControl,LEN_RED_Close,2500,BatState);	// 3秒闪一次
	
		}
		OS_AddTask(tasklink,taskBatControl);// 添加电池电量检测任务
	}
}

