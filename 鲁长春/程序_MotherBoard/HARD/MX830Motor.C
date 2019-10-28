#include "UHEAD.H"
#include "MX830Motor.h"
#include "ADC_CHECK.H"
#include "lowpower.h"
#include "keyboard.h"
#include "FL.H"
#include "stmflash.h"
#include "BAT.H"
#include "LED_SHOW.H"
Motor_Struct    	motorStruct 	= {0};      	// 马达状态结构体
TaskStr* 		taskMotor 	= {0};		// 马达运动任务
u32 			shut_time 	= 0;		// 之前的版本是保存关窗的时间，现在改为保存窗的BH位置：hasrun
u8			flag_motorIO 	= 0;		// 马达引脚调换标志
u8			flag_YS_isno 	= 0;		// FLYS无检测
WindowState		windowstate 	= open;
u8 			flag_flag_YS_SHUT = 0;
u8			flag_bat2BC1 	= 0;		// 执行无条件关窗的标志位

extern	TaskLinkStr* 	tasklink;		// 任务列表
extern	u8 		flag_KEY_Z;		// 传递给马达函数，让他根据val做出动作
extern	u8 		flag_KEY_Y;
extern	keyStr 		key_Z;
extern	keyStr 		key_Y;
extern	u32 		dm_counter;		// 开机检测DM，计数器
extern	u8 		flag_BHProtectStep;
extern 	u32 		counter_BH;		// BH计数
extern 	u8 		flag_no30;
extern 	u32 		threshold_30 ;
extern 	TimerLinkStr 	timer2;
extern 	u8		flag_FLreasion;
extern 	u8 		flag_FL_SHUT;
extern	BATStr 		bat;			// 电池管理
extern	u8		ys_timer30;		// YS不响应计时
extern	JugeCStr 	YS_30;
extern	TaskStr* 	taskAlarm; 
/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/

void MX830Motor_GPIOInit()
{
    GPIO_Init(MX830Motor_GPIO_FI,MX830Motor_GPIOMode);
    GPIO_Init(MX830Motor_GPIO_BI,MX830Motor_GPIOMode);
}
 
void Motor_Forward()
{
	if(flag_motorIO)
	{
		GPIO_SetBits(MX830Motor_GPIO_FI);
		GPIO_ResetBits(MX830Motor_GPIO_BI);
	}else
	{
		GPIO_SetBits(MX830Motor_GPIO_BI);
		GPIO_ResetBits(MX830Motor_GPIO_FI);		
	}
}

void Motor_Back()
{					

	if(flag_motorIO)
	{
		GPIO_SetBits(MX830Motor_GPIO_BI);
		GPIO_ResetBits(MX830Motor_GPIO_FI);
	}else
	{
		GPIO_SetBits(MX830Motor_GPIO_FI);
		GPIO_ResetBits(MX830Motor_GPIO_BI);	
	}					
}
/**************************************
*@brief
*@note 
*@param
*@retval
***************************************/

void MX830Motor_StateDir(Motor_Struct* motorStruct)
{ 
    if(motorStruct->command != MOTOR_NULL)
	{
		
		if(motorStruct->dir != motorStruct->command)	//换向
		{
			//debug("马达换向\r\n");  
			motorStruct->dir = motorStruct->command;
			motorStruct->counter = 0;
		
			switch(motorStruct->dir)
			{
				case FORWARD    :     
						debug("dir = FORWARD\r\n");           
						Motor_Forward();
						motorStruct->dirBack = FORWARD;
				  break;
				case BACK       :        
						debug("dir = BACK\r\n"); 
						Motor_Back();
						motorStruct->dirBack = BACK;
				  break;
				case STOP       :  
				  {       
						debug("dir = STOP\r\n");
						GPIO_ResetBits(MX830Motor_GPIO_FI);
						GPIO_ResetBits(MX830Motor_GPIO_BI);
				  }          
				  break;
				case HOLD       :  
				  GPIO_SetBits(MX830Motor_GPIO_FI); 
				  GPIO_SetBits(MX830Motor_GPIO_BI);
				  break;
			}
		}
		motorStruct->command = MOTOR_NULL;
	}

}
           
void MotorForword_BC1()
{
	motorStruct.command = FORWARD;
	MX830Motor_StateDir(&motorStruct);
}
void MotorForword_BC2()
{
	motorStruct.command = BACK;
	MX830Motor_StateDir(&motorStruct);
}
void MotorBACK_BC1()
{
	motorStruct.command = BACK;
	MX830Motor_StateDir(&motorStruct);
}
void MotorBACK_BC2()
{
	motorStruct.command = FORWARD;
	MX830Motor_StateDir(&motorStruct);
}

void LEN_GREEN_Close();

void MotorSTOP()
{	
	motorStruct.command = STOP;
	MX830Motor_StateDir(&motorStruct);
}


void Motor_Z()
{
	motorStruct.command = FORWARD;
	MX830Motor_StateDir(&motorStruct);
}

void Motor_Y()
{
	motorStruct.command = BACK;
	MX830Motor_StateDir(&motorStruct);
}

void Motor_RunBack()
{
	if(motorStruct.dirBack == FORWARD) motorStruct.command = BACK;
	else motorStruct.command = FORWARD;
	counter_BH = 0;
	MX830Motor_StateDir(&motorStruct);
}

void Motor_RunFORWARD()
{
	if(motorStruct.dirBack == FORWARD) motorStruct.command = FORWARD;
	else motorStruct.command = BACK ;
	counter_BH = 0;
	MX830Motor_StateDir(&motorStruct);
}

void OpenWindow()
{
	Motor_Y();
}

void ShutDownWindow()
{
	Motor_Z();
}


bool MotorSysProtect0()		//最高优先级保护：电机转动保护，电压过低
{
	return (bool)(motorStruct.erro & 0x0f);	// 除掉BH方波保护
}
bool MotorBHAppear()
{
	return(bool)(MotorSysProtect0()|| motorStruct.erro&ERROR_BH == 0 );
}

bool MotorSysProtect1()		// 次优先级保护：电机转动保护，电压过低，BH方波保护
{
	return (bool)(motorStruct.erro);
}

bool FL_MotorProtect()		// FL运行保护：电机转动保护，电压过低，BH方波保护，到达BC1
{
	return (bool)(motorStruct.erro || (motorStruct.flag_BC1 && motorStruct.dir == FORWARD));
}


bool MotorProtectKey()		// 按键优先级保护: 电机转动保护，电压过低，BH方波保护,ZY按键,BC1,BC2限位
{
	return (bool)(MotorSysProtect1()||flag_KEY_Z||flag_KEY_Y|| (motorStruct.hasrun == motorStruct.needrun)\
	    ||(motorStruct.flag_BC1 && motorStruct.dir == FORWARD)||(motorStruct.flag_BC2 && motorStruct.dir == BACK));
}

bool MotorProtectHold()		// 按键优先级保护: 电机转动保护，电压过低，BH方波保护,ZY按键,BC1,BC2限位
{
	return (bool)(MotorSysProtect1()||flag_KEY_Z||flag_KEY_Y\
	    ||(motorStruct.flag_BC1)||(motorStruct.flag_BC2)|| motorStruct.erro&ERROR_BH);
}
void Motor_AutoRun()
{
	if(motorStruct.hasrun > motorStruct.needrun) Motor_Z();
	else if(motorStruct.hasrun < motorStruct.needrun) Motor_Y();
}


bool MotorProtectAM()	//AM模式下，除了Y30电机转动保护，充电保护，BH方波保护，达到AM位置
{
	return (bool)(MotorSysProtect1() || (motorStruct.hasrun == shut_time) || key_AM.val == on);	

}


bool YS_MotorProtect()	//电机转动保护，充电保护，BH方波保护，Y30延时,关窗到限位
{
	bool juge = (bool)((motorStruct.counter > MOTOR_F_SAFE) \
		|| GPIO_READ(CHARGE_PRO_PIN) != RESET || counter_BH > BH_SAFE || YS_30.start == 1 ||(motorStruct.flag_BC1 && motorStruct.dir == FORWARD));
	if(juge)
	{
		jugeYS.switchon = 0;
		flag_flag_YS_SHUT = 0;
	}
	return juge;
}

void WindowStateBC1()
{
	windowstate = to_BC1;
}
void WindowStateBC2()
{
	windowstate = to_BC2;
}

void ResetBHErro()
{
    	counter_BH = 0;
	motorStruct.erro &= ~ERROR_BH;
}
void MotorHold()
{
 	Motor_RunBack(); //回退到BH1,BH2高电平的位置再停止
	while((GPIO_READ(GPIO_BH) == RESET || GPIO_READ(GPIO_BH2) == RESET)&& ((MotorSysProtect1()||flag_KEY_Z||flag_KEY_Y)==FALSE));
	motorStruct.command = HOLD;
	MX830Motor_StateDir(&motorStruct);	
}

bool Juge_y30_end()
{
	return (bool)(YS_30.start == 0);
}
void AlarmMotor()
{
	OS_AddJudegeFunction(taskAlarm,LEN_REDBEEP_Open,100,Juge_y30_end);	
	OS_AddJudegeFunction(taskAlarm,LEN_REDBEEP_Close,100,Juge_y30_end);	
	OS_AddJudegeFunction(taskAlarm,LEN_GREENBEEP_Open,100,Juge_y30_end);	
	OS_AddJudegeFunction(taskAlarm,LEN_GREENBEEP_Close,100,Juge_y30_end);	
	OS_AddCycleFunction(taskAlarm,6);
	OS_AddTask(tasklink,taskAlarm);
}
//马达运动
void MotorControl()
{
	static u8 flag_motor_erro = 0; 
	//充电状态禁止转动，超过转动时限，BAT电压过低禁止转动,除去BH无方波
	if(MotorSysProtect0())
	{
		motorStruct.flag_BC1 = 0;
		motorStruct.flag_BC2 = 0;
		if(flag_motor_erro == 0)
		{
			flag_motor_erro = 1;
			debug("超过转动时限，BAT电压过低禁止转动\r\n");
			if(motorStruct.erro&ERROR_MOTOR)
			{
				OS_AddJudegeFunction(taskMotor,AlarmMotor,TIM_ALARM_BH,Juge_y30_end);	
				OS_AddTask(tasklink,taskMotor);			
			}

		}
	}
	else
	{
		flag_motor_erro = 0;
		if((taskMotor->state == Stop))
		{
			if((motorStruct.erro & ERROR_BH)&& flag_BHProtectStep == 0 && flag_no30 == 0)				// 方波超时保护
			{
			    	
				debug("方波超时保护\r\n");
				flag_BHProtectStep = 1;													//如果出现BH，则会在中断中清零
				counter_BH = 0;
				//按马达当前转向，反向旋转4S，再继续原来转向
				OS_AddJudegeFunction(taskMotor,Motor_RunFORWARD,TIM_BH0,MotorBHAppear);	// 反向旋转4S
				OS_AddJudegeFunction(taskMotor,Motor_RunBack,IRQ_PERIOD,MotorBHAppear);	//恢复原来的转向
				OS_AddFunction(taskMotor,ResetBHErro,IRQ_PERIOD);	
				OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务
				OS_AddTask(tasklink,taskMotor);						// 添加到任务队列
			}
			else if((motorStruct.erro & ERROR_BH) && flag_BHProtectStep == 1)		//再次出现无方波
			{
				debug("再次出现无方波\r\n");
				
				MotorSTOP();
				flag_BHProtectStep = 0;	
				counter_BH = 0;
				FL_CheckStop();
				flag_no30 = 1;		// 30分钟不响应YSFL信号
				ys_timer30 = TIM_30;
				YS_30.start = 1;	//此参数终止YS检测
				YS_30.counter = 0;
				motorStruct.erro &= ~ERROR_BH;
			OS_AddJudegeFunction(taskMotor,AlarmMotor,TIM_ALARM_BH,Juge_y30_end);	
			OS_AddTask(tasklink,taskMotor);
			}else
			 if(flag_KEY_Z)//按键<Z
			{
				flag_KEY_Z = 0;	
				flag_bat2BC1 = 0;
				debug("key_Z.val = %d\r\n",key_Z.val);					// 移除任务
				switch(key_Z.val)
				{
					case off:
					break;
					case on:
						motorStruct.needrun = - 1000;			// 执行<z
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
					break;
					case two:	
						motorStruct.needrun = motorStruct.hasrun - STEP_MOTO;
						debug("flag_KEY_Z :two\r\n");
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
						
					break;
					default:// 关2/3
						motorStruct.needrun = motorStruct.hasrun - STEP_MOTO - STEP_MOTO;
						debug("flag_KEY_Z :three\r\n");	
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
					break;
				    
				}

			//	debug("motorStruct.hasrun =%d,motorStruct.needrun =%d\r\n",motorStruct.hasrun,motorStruct.needrun);										
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);		// 移除任务	
				OS_AddTask(tasklink,taskMotor);			// 添加到任务队列
			}
			else
			if(flag_KEY_Y)//按键>Y
			{
				flag_KEY_Y = 0;
				flag_bat2BC1 = 0;
				debug("key_Y.val = %d\r\n",key_Y.val);
				switch(key_Y.val)
				{
					case off:
					break;
					case on:
						motorStruct.needrun = 1000;			// 执行>Y
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
					break;
					case two:// 开1/3
						motorStruct.needrun = motorStruct.hasrun + STEP_MOTO;
						debug("KEY_Y :two\r\n");
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
					break;
					default:// 开2/3
						debug("KEY_Y :three\r\n");
						motorStruct.needrun = motorStruct.hasrun + STEP_MOTO + STEP_MOTO;
						OS_AddJudegeFunction(taskMotor,Motor_AutoRun,MOTOR_F_SAFE,MotorProtectKey);
					break;	
				}					
		
				//debug("motorStruct.hasrun =%d,motorStruct.needrun =%d\r\n",motorStruct.hasrun,motorStruct.needrun);								
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务	
				OS_AddTask(tasklink,taskMotor);						// 添加到任务队列
			}	
			else if((motorStruct.flag_BC1||motorStruct.flag_BC2) && motorStruct.flag_BC == 0 ) // 窗限位动作
			{
				debug("窗限位动作:");
				motorStruct.flag_BC = 1;
				if(motorStruct.flag_BC1)						// 关窗限位
				{
					debug("关窗限位\r\n");
					OS_AddJudegeFunction(taskMotor,MotorForword_BC1,TIM_MOTOR_Z,MotorSysProtect1);	// 正转1秒
					OS_AddJudegeFunction(taskMotor,Motor_RunBack,TIM_MOTOR_F,MotorSysProtect0);;	// 反转1.5秒
					OS_AddFunction(taskMotor,WindowStateBC1,IRQ_PERIOD);				// 关窗标志置位
					motorStruct.flag_BC1 = 0;	
					key_Z.counter = 0;
					motorStruct.hasrun = 0;
					motorStruct.needrun = 0;
					if(shut_time != 0)	//计算关窗用的时间
					{
						//没有FLYS时，启动定时器计时，延时打开窗
						flag_YS_isno = 1;
					}
					jugeYS.switchon = 0;	//不进行Y30按键检测
					flag_flag_YS_SHUT = 0;
				}else
				if(motorStruct.flag_BC2)
				{
					debug("开窗限位\r\n");
					OS_AddJudegeFunction(taskMotor,MotorForword_BC2,TIM_MOTOR_Z,MotorSysProtect1);	// 正转1秒
					OS_AddJudegeFunction(taskMotor,Motor_RunBack,TIM_MOTOR_F,MotorSysProtect0);	// 反转1.5秒
					OS_AddFunction(taskMotor,WindowStateBC2,IRQ_PERIOD);				// 关窗标志置位
					motorStruct.flag_BC2 = 0;	
					key_Y.counter = 0;
					motorStruct.hasrun = 0;
					motorStruct.needrun = 0;
				}
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);	
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务
				OS_AddTask(tasklink,taskMotor);							// 添加到任务队列
			}		
			//YS达到阀值，关窗
			else if(jugeYS.switchon && (YS_30.start==0))
			{
				if(flag_flag_YS_SHUT == 0)
				{
					flag_flag_YS_SHUT = 1;
					debug("\r\nYS达到阀值，关窗\r\n");
					OS_AddFunction(taskMotor,OS_DeleteTask,0);				// 移除任务
					OS_AddJudegeFunction(taskMotor,ShutDownWindow,MOTOR_F_SAFE,YS_MotorProtect);// 执行关窗
					OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
					OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务
					OS_AddTask(tasklink,taskMotor);						// 添加到任务队列
					if(key_AM.val == on)
					{
						shut_time = motorStruct.hasrun;	// AM下自动记录关窗时间	
						debug("\r\nshut_time = %lu\r\n",shut_time);
					}					
				}
			}else
			if(flag_FL_SHUT)//FL达到阀值，关窗
			{
				debug("FL达到阀值，关窗\r\n");
				flag_FL_SHUT = 0;
				OS_AddJudegeFunction(taskMotor,ShutDownWindow,MOTOR_F_SAFE,FL_MotorProtect);// 执行关窗
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务				
				OS_AddTask(tasklink,taskMotor);						// 添加到任务队列			
			}
			else
			if(jugeYS_No.switchon)								//无YS,FL开窗
			{
				debug("无YS开窗\r\n");
				jugeYS_No.switchon = 0;
				flag_bat2BC1 = 0;
				OS_AddJudegeFunction(taskMotor,OpenWindow,MOTOR_F_SAFE,MotorProtectAM);	// 执行开窗
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务	
				OS_AddTask(tasklink,taskMotor);						// 添加到任务队列
			}
			else if(bat.state == BAT_STATE_38BC1 && flag_bat2BC1 == 0)
			{
				debug("电池电压低，无条件关窗\r\n");
				flag_bat2BC1 = 1;
				OS_AddJudegeFunction(taskMotor,ShutDownWindow,MOTOR_F_SAFE,FL_MotorProtect);// 执行关窗
				OS_AddJudegeFunction(taskMotor,MotorHold,TIM_MOTO_HOLD,MotorProtectHold);
				OS_AddFunction(taskMotor,MotorSTOP,0);					// 移除任务				
				OS_AddTask(tasklink,taskMotor);						// 添加到任务队列			
				
			}
		}		
	}
}

//BC1,BC2
WindowState CheckWindowState()
{
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	if(GPIO_READ(GPIO_38KHZ_BC1) == RESET && motorStruct.dir == FORWARD)
	{
		windowstate = to_BC1;
	}
	else if(GPIO_READ(GPIO_38KHZ_BC2) == RESET && motorStruct.dir == BACK)windowstate = to_BC2;
	else if(GPIO_READ(GPIO_38KHZ_BC1) != RESET && GPIO_READ(GPIO_38KHZ_BC2) != RESET)
	{
		windowstate = open;
	}
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_Out_PP_Low_Slow);
	return windowstate;
}
void CheckBC1BC2()
{
	
	if((motorStruct.dir != STOP) && (motorStruct.flag_BC == 0))	
	{	
		CheckWindowState();
		if(windowstate == to_BC1 &&  motorStruct.dir == FORWARD) 
		{
		    	debug("motorStruct.flag_BC1 = 1\r\n");
			motorStruct.flag_BC1 = 1;
		}
		if(windowstate == to_BC2 && motorStruct.dir == BACK)
		{
			debug("motorStruct.flag_BC2 = 1\r\n");
			motorStruct.flag_BC2 = 1;		
		} 
	}

	if(motorStruct.flag_BC && taskMotor->state == Stop)
	{
	    	debug("motorStruct.flag_BC = 0\r\n");
		motorStruct.flag_BC = 0;	
	} 

}














