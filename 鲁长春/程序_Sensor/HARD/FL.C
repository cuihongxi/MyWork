#include "FL.H"

u32 	GetSysTime();
u32 	fl_speed_width 	= (60000/VALVE_FLSPEED);	// 根据转速阀值计算间隔,ms
u8		flag_FL_SHUT 	= 1;
u8		flag_FLCheckState = 0;				//fl检测开始还是停止
void FL_GPIO_Init()
{
	FL_CheckStart();
	disableInterrupts();
	EXTI_SetPinSensitivity(EXTI_Pin_3,EXTI_Trigger_Falling);
    enableInterrupts();					// 使能中断
}

void FL_CheckStart()
{
    flag_FLCheckState = 1;
	GPIO_Init(GPIO_FLU,GPIO_Mode_In_PU_IT);  
	debug("FL_CheckStart->\r\n");
}

void FL_CheckStop()
{
    flag_FLCheckState = 0;
	GPIO_Init(GPIO_FLU,GPIO_Mode_Out_PP_Low_Slow);  
	debug("FL_CheckStop-->\r\n");
}


void FL_Check()
{
	static u32 	counter_FL = 0;					// FL计数器
	static u32 	counter_filter = 0;				// 滤波计数器
	static u8 flag = 0;							//第一次达到转速时启动计时标志
	static u32 timer = 0;						//转速维持4S的计时
	
	if(flag_FLCheckState && GPIO_READ(GPIO_FLU) == RESET )
	{
		debug("GPIO_FLU = 0\r\n");
		u32 counter = GetSysTime();
		if(counter < counter_FL && counter > counter_filter) 		//超过阀值，转速有效，等待维持4s
		{
			if(flag == 0)		
			{
				flag = 1;					//启动持续计时
				timer = counter;
				debug("FL:启动持续计时\r\n");
			}else
			{
				if((counter - timer)>TIM__FL_D)
				{
				    debug("in it 	FL:持续时间到，关窗\r\n");
					flag_FL_SHUT = 1;			// 持续时间到，关窗
					FL_CheckStop();
				}	
			}
			
		}else
		{
			flag = 0;
		}
		counter_filter = counter + TIM_FL_FILTER;
		counter_FL = counter + fl_speed_width;

	}
}


INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{
	FL_Check();	
    EXTI_ClearITPendingBit (EXTI_IT_Pin3);
}
