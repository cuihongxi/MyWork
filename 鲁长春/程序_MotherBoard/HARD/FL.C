#include "FL.H"
#include "lowpower.h"
u8 	flag_30 = 0;
u8 		flag_no30 = 0;		//	30分钟不响应YSFL
u32 	fl_speed_width	= (60000/VALVE_FLSPEED);	// 根据转速阀值计算间隔,ms
u32 	counter_BH	= 0;	//BH计数

extern	TimerLinkStr 	timer2 ;					// 任务的定时器
extern	u8 				flag_YS_SHUT ;
void FL_GPIO_Init()
{
	GPIO_Init(GPIO_FLU,GPIO_Mode_In_PU_IT);  
	disableInterrupts();
	EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);
    enableInterrupts();                                                 // 使能中断
}


INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
	static u32 	counter_FL = 0;					// FL计数器
	static u8 flag = 0;							//第一次达到转速时启动计时标志
	static u32 timer = 0;						//转速维持4S的计时
	
	if(GPIO_READ(GPIO_FLU) == RESET)
	{
		u32 counter = GetSysTime(&timer2);
		if(counter_FL == 0) counter_FL = counter + fl_speed_width;
		else 
		{
			if(counter < counter_FL) //超过阀值，转速有效，等待维持4s
			{
				if(flag == 0)		
				{
					flag = 1;			//启动滤波计时
					timer = counter;
				}else
				{
					if((counter - timer)>(TIM__FL_D*1000)) flag_YS_SHUT = 1;	//滤波时间到，关窗
				}
				
			}else
			{
				flag = 0;
			}
			
			counter_FL = counter + fl_speed_width;
		}
	}
	debug("EXTI6_IRQHandler\r\n");
    EXTI_ClearITPendingBit (EXTI_IT_Pin6);
}


/******************************BH检测***********************************/

void BH_CheckStart()
{
	GPIO_Init(GPIO_BH,GPIO_Mode_In_PU_IT);
	disableInterrupts();
	EXTI_SetPinSensitivity(EXTI_Pin_0,EXTI_Trigger_Falling);
    enableInterrupts();  
	
}

void BH_CheckStop()
{
	GPIO_Init(GPIO_BH,GPIO_Mode_In_PU_No_IT);
	counter_BH = 0;
}

INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
	if(GPIO_READ(GPIO_BH) == RESET)
	{
		counter_BH = 0;
	}
	if(flag_30 == 1 )flag_30 = 0;	// 恢复故障
	debug("EXTI0_IRQHandler\r\n");
	EXTI_ClearITPendingBit (EXTI_IT_Pin0);
}


