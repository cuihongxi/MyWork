#include "FL.H"
#include "MX830Motor.h"
#include "CUI_RTOS.H"
#include "LED_SHOW.H"

u8 		flag_BHProtectStep = 0;				// BH方波保护执行的步骤	
u8 		flag_no30 = 0;					// BH故障，30分钟不响应YSFL
u32 		fl_speed_width 	= (60000/VALVE_FLSPEED);	// 根据转速阀值计算间隔,ms
u32 		counter_BH	= 0;				//BH计数
u8		flag_FLreasion 	= 0;				// FL的原因关窗
u8		flag_FL_SHUT 	= 0;
BH_dir		motor_BHdir 	=  BH_Stop;			//记录窗户运动方向
u8		flag_BH 	= 0;				//双BH 正方向标志
u8		flag_FLCheckState = 0;				//fl检测开始还是停止
JugeCStr	jugeWindows = {0};				
JugeCStr	jugeBHLED = {0};
extern	TimerLinkStr 	timer2 ;				// 任务的定时器
extern	u8 		flag_BHLED;
extern	u8		flag_bat2BC1;
extern 	u8		flag_DM;

void BH_FL_GPIO_Init()
{
	FL_CheckStart();
	BH_CheckStart();

	disableInterrupts();
	EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);
	EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
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

void FL_Control()
{
    if(flag_FLCheckState == 0 && windowstate != SHUTDOWN && motorStruct.dir == STOP &&  flag_no30 == 0 && flag_FL_SHUT == 0)
    {
		FL_CheckStart();		
    }
}


void FL_Check()
{
	static u32 	counter_FL = 0;					// FL计数器
	static u32 	counter_filter = 0;				// 滤波计数器
	static u8 flag = 0;						//第一次达到转速时启动计时标志
	static u32 timer = 0;						//转速维持4S的计时
	
	if(flag_FLCheckState && GPIO_READ(GPIO_FLU) == RESET )
	{
		debug("GPIO_FLU = 0\r\n");
		u32 counter = GetSysTime(&timer2);

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

/******************************BH检测***********************************/

void BH_CheckStart()
{
	GPIO_Init(GPIO_BH,GPIO_Mode_In_PU_IT);
	GPIO_Init(GPIO_BH2,GPIO_Mode_In_PU_No_IT);
}

//在BH下降中断中获得方向
BH_dir BH_GetDir()
{
	if(GPIO_READ(GPIO_BH2)) return BH_Close ;
	else return BH_Open;
}
void BH_CheckStop()
{
	debug("BH_CheckStop\r\n");
	GPIO_Init(GPIO_BH,GPIO_Mode_Out_PP_Low_Slow);
	counter_BH = 0;
}

void BH_Check()
{
	if(GPIO_READ(GPIO_BH) == RESET)
	{
		
		//滤波		
		static u32 	counterFileter_BH = 0;
		u32 counter = GetSysTime(&timer2);
		if(counter > counterFileter_BH)
		{
			LEN_GREEN_Open();
			jugeBHLED.start = 1;
			jugeBHLED.counter = 0;
			jugeWindows.start = 1;
			jugeWindows.counter = 0;
			motor_BHdir = BH_GetDir();
			if(motor_BHdir == BH_Open) 
			{
				motorStruct.hasrun ++;

				debug("++\r\n");
				if(flag_DM == 0)
				{
					if(windowstate == SHUTDOWN)	//检测手动开窗
					{
						static u8 i = 1;
						static int j = 0;
						if(i)
						{
							i = 0;
							j = motorStruct.hasrun;
						}
						if((motorStruct.hasrun - j)==0)
						{
							j = 0;i = 1;
							debug("开窗\r\n");
							flag_bat2BC1 = 0;
							windowstate = open;				    
						}
					}				
				}else
				{
					if(windowstate == OPENDOWN)	//检测手动关窗
					{
						static u8 i = 1;
						static int j = 0;
						if(i)
						{
							i = 0;
							j = motorStruct.hasrun;
						}
						if((j - motorStruct.hasrun)==0)
						{
							j = 0;i = 1;
							debug("关窗\r\n");
							windowstate = open;				    
						}
					}				
				}
	
				

				}
			else 
			{
			   
				motorStruct.hasrun --;
				debug("--\r\n");
				if(flag_DM == 0)
				{
					if(windowstate == OPENDOWN)	//检测手动关窗
					{
						static u8 i = 1;
						static int j = 0;
						if(i)
						{
							i = 0;
							j = motorStruct.hasrun;
						}
						if((j - motorStruct.hasrun)==0)
						{
							j = 0;i = 1;
							debug("关窗\r\n");
							windowstate = open;				    
						}
					}				
				}else
				{
					if(windowstate == SHUTDOWN)	//检测手动开窗
						{
							static u8 i = 1;
							static int j = 0;
							if(i)
							{
								i = 0;
								j = motorStruct.hasrun;
							}
							if((motorStruct.hasrun - j)==0)
							{
								j = 0;i = 1;
								debug("开窗\r\n");
								flag_bat2BC1 = 0;
								windowstate = open;				    
							}
						}				
				}

			}
			counter_BH = 0;					// 清零BH计时，否则被马达认为没有转动	
			if(motorStruct.erro & ERROR_BH)
			{
			  	static int	run = 0;
				u8 i = 0;
				if(run == 0) run = motorStruct.hasrun;
				else
				{
					if(motorStruct.hasrun > run) i = motorStruct.hasrun - run;
					else i = run - motorStruct.hasrun;
					debug("i = %d\n",i);
				}
				if(i > 1)
				{
				  	debug("BH故障恢复\n");
				  	run = 0;
					motorStruct.erro &= ~ERROR_BH;
					if(flag_BHProtectStep == 1 )flag_BHProtectStep = 0;	// 恢复故障				
				}

			} 
		}
		
		counterFileter_BH = counter + TIM_BH_FILTER;
			
	}
  
//	if(GPIO_READ(GPIO_BH) == RESET)
//	{
//		
//		//滤波		
//		static u32 	counterFileter_BH = 0;
//		u32 counter = GetSysTime(&timer2);
//		if(counter > counterFileter_BH)
//		{
//			LEN_GREEN_Open();
//			jugeBHLED.start = 1;
//			jugeBHLED.counter = 0;
//			jugeWindows.start = 1;
//			jugeWindows.counter = 0;
//			motor_BHdir = BH_GetDir();
//			if(motor_BHdir == BH_Open) 
//			{
//				motorStruct.hasrun ++;
//
//				debug("++\r\n");
//				if(flag_DM == 0)
//				{
//					if(windowstate == SHUTDOWN)	//检测手动开窗
//					{
//						static u8 i = 1;
//						static int j = 0;
//						if(i)
//						{
//							i = 0;
//							j = motorStruct.hasrun;
//						}
//						if((motorStruct.hasrun - j)==0)
//						{
//							j = 0;i = 1;
//							debug("开窗\r\n");
//							flag_bat2BC1 = 0;
//							windowstate = open;				    
//						}
//					}				
//				}else
//				{
//					if(windowstate == OPENDOWN)	//检测手动关窗
//					{
//						static u8 i = 1;
//						static int j = 0;
//						if(i)
//						{
//							i = 0;
//							j = motorStruct.hasrun;
//						}
//						if((j - motorStruct.hasrun)==0)
//						{
//							j = 0;i = 1;
//							debug("关窗\r\n");
//							windowstate = open;				    
//						}
//					}				
//				}
//	
//				
//
//				}
//			else 
//			{
//			   
//				motorStruct.hasrun --;
//				debug("--\r\n");
//				if(flag_DM == 0)
//				{
//					if(windowstate == OPENDOWN)	//检测手动关窗
//					{
//						static u8 i = 1;
//						static int j = 0;
//						if(i)
//						{
//							i = 0;
//							j = motorStruct.hasrun;
//						}
//						if((j - motorStruct.hasrun)==0)
//						{
//							j = 0;i = 1;
//							debug("关窗\r\n");
//							windowstate = open;				    
//						}
//					}				
//				}else
//				{
//					if(windowstate == SHUTDOWN)	//检测手动开窗
//						{
//							static u8 i = 1;
//							static int j = 0;
//							if(i)
//							{
//								i = 0;
//								j = motorStruct.hasrun;
//							}
//							if((motorStruct.hasrun - j)==0)
//							{
//								j = 0;i = 1;
//								debug("开窗\r\n");
//								flag_bat2BC1 = 0;
//								windowstate = open;				    
//							}
//						}				
//				}
//
//			}
//			counter_BH = 0;					// 清零BH计时，否则被马达认为没有转动	
//			if(motorStruct.erro & ERROR_BH) motorStruct.erro &= ~ERROR_BH;
//			if(flag_BHProtectStep == 1 )flag_BHProtectStep = 0;	// 恢复故障
//		}
//		
//		counterFileter_BH = counter + TIM_BH_FILTER;
//			
//	}
}


INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
	FL_Check();	
	
	//debug("EXTI6_IRQHandler\r\n");
    	EXTI_ClearITPendingBit (EXTI_IT_Pin6);
}

#ifndef  DM_LED_RESET  
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{

        BH_Check();//BH检测
	EXTI_ClearITPendingBit (EXTI_IT_Pin7);
}
#endif