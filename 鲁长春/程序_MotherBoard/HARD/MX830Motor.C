#include "UHEAD.H"
#include "MX830Motor.h"
#include "ADC_CHECK.H"
#include "lowpower.h"
#include "keyboard.h"
#include "FL.H"
#include "stmflash.h"
#include "BAT.H"

Motor_Struct    		motorStruct = {0};      // 马达状态结构体
TaskStr* 				taskMotor = {0};		// 马达运动任务
u32 					shut_time = 0;
u8						flag_motorIO = 0;		// 马达引脚调换标志
u8						flag_YS_isno = 0;		// YS无检测
WindowState				windowstate = open;

extern	TaskLinkStr* 	tasklink;				// 任务列表
extern	u8 				flag_KEY_Z ;			// 传递给马达函数，让他根据val做出动作
extern	u8 				flag_KEY_Y ;
extern	keyStr 			key_Z ;
extern	keyStr 			key_Y;
extern	u32 			dm_counter;				// 开机检测DM，计数器
extern	u8 				flag_30 ;
extern 	u32 			counter_BH	;			// BH计数
extern 	u8 				flag_no30;
extern 	u32 			threshold_30 ;
extern 	TimerLinkStr 	timer2;
extern 	u8				flag_FLreasion;
extern 	u8 				flag_FL_SHUT;
extern	BATStr 			bat;					// 电池管理
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
			//if(motorStruct->dir != STOP) 待解决，旋转60秒后怎么复位？
				motorStruct->counter = 0;
		}
		switch(motorStruct->dir)
		{
			case FORWARD    :     
				//	debug("dir = FORWARD\r\n");           
					Motor_Forward();
			  	//	BH_CheckStart();
			  break;
			case BACK       :        
			//debug("dir = BACK\r\n"); 
					Motor_Back();
				//	BH_CheckStart();
			  break;
			case STOP       :  
			  {       
					debug("dir = STOP\r\n");
					GPIO_ResetBits(MX830Motor_GPIO_FI);
					GPIO_ResetBits(MX830Motor_GPIO_BI);
					key_Y.counter = 0;
					key_Z.counter = 0;
					BH_CheckStop();
			  }          
			  break;
			case HOLD       :  
			  GPIO_SetBits(MX830Motor_GPIO_FI); 
			  GPIO_SetBits(MX830Motor_GPIO_BI);
			  break;
		}		
		motorStruct->command = MOTOR_NULL;
	}

}
           
void MotorForword_BC1()
{
	motorStruct.command = FORWARD;
}
void MotorForword_BC2()
{
	motorStruct.command = BACK;
}
void MotorBACK_BC1()
{
	motorStruct.command = BACK;
}
void MotorBACK_BC2()
{
	motorStruct.command = FORWARD;
}
void MotorSTOP()
{
	motorStruct.command = STOP;
}

void Motor_Z()
{
	motorStruct.command = FORWARD;
}

void Motor_Y()
{
	motorStruct.command = BACK;
}

void BH_Motor1()
{
	if(motorStruct.dir == FORWARD) motorStruct.command = BACK;
	else motorStruct.command = FORWARD;
	counter_BH = 0;
}

void OpenWindow()
{
	Motor_Y();
}

void ShutDownWindow()
{
	Motor_Z();
}


bool MotorProtectWithY30()	//电机转动保护，充电保护，BH方波保护，Y30延时
{
	return (bool)(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) \
		|| GPIO_READ(CHARGE_PRO_PIN) != RESET || counter_BH > BH_SAFE || YS_30.start);
}

bool MotorProtect()	//除了Y30电机转动保护，充电保护，BH方波保护
{
	return (bool)(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) \
		|| GPIO_READ(CHARGE_PRO_PIN) != RESET || counter_BH > BH_SAFE);
}

bool YSFL_MotorProtect()	//电机转动保护，充电保护，BH方波保护，Y30延时,关窗到限位
{
	return (bool)(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) \
		|| GPIO_READ(CHARGE_PRO_PIN) != RESET || counter_BH > BH_SAFE || YS_30.start == 1 ||  CheckWindowState() == to_BC1);
}

void WindowStateBC1()
{
	windowstate = to_BC1;
}
void WindowStateBC2()
{
	windowstate = to_BC2;
}
//马达运动
void MotorControl()
{
	static u8 flag_motor_erro = 0;
	static	u8 flag_flag_FL_SHUT = 0;
	//充电状态禁止转动，超过转动时限，BAT电压过低禁止转动
	if(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) || GPIO_READ(CHARGE_PRO_PIN) != RESET || flag_no30 == 1 || bat.state == BAT_STATE_NoBACK)
	{
		if(flag_motor_erro == 0)
		{
			flag_motor_erro = 1;
			motorStruct.command = STOP;	//正反转保护时间判断
			motorStruct.flag_BC1 = 0;
			motorStruct.flag_BC2 = 0;		
			debug("充电状态禁止转动，超过转动时限，BAT电压过低禁止转动\r\n");
		}
	}
	else
	{
		flag_motor_erro = 0;
		//所有情况下马达正转至38BC1高电平，延时1秒（可调）后反转1.5秒（可调）停转；\
		反转至38BC2高电平，延时1秒正转1.5秒停转。
		if((taskMotor->state == Stop)) // taskMotor->state == Wait || 
		{
			if(counter_BH > BH_SAFE && flag_30 == 0)							// 方波超时保护
			{
				debug("方波超时保护\r\n");
				flag_30 = 1;													//如果出现BH，则会在中断中清零
				counter_BH = 0;
				//按马达当前转向，反向旋转4S，再继续原来转向
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddJudegeFunction(taskMotor,BH_Motor1,TIM_BH0,MotorProtect);	// 反向旋转4S
				OS_AddJudegeFunction(taskMotor,BH_Motor1,10,MotorProtect);		//恢复原来的转向
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
			}
			else if(counter_BH > BH_SAFE && flag_30 == 1)						//再次出现无方波
			{
				debug("再次出现无方波\r\n");
				motorStruct.command = STOP;	//正反转保护时间判断
				//30分钟不响应YS信号
				flag_no30 = 1;
				threshold_30 = GetSysTime(&timer2) + TIM_30;
				flag_30 = 0;
				counter_BH = 0;
			}
			else if((motorStruct.flag_BC1||motorStruct.flag_BC2) && motorStruct.flag_BC == 0) // 窗限位动作
			{
				debug("窗限位动作:");
				motorStruct.flag_BC = 1;
				OS_AddFunction(taskMotor,OS_DeleteTask,0);				// 移除任务
				if(motorStruct.flag_BC1)								// 关窗限位
				{
					debug("关窗限位\r\n");
					OS_AddJudegeFunction(taskMotor,MotorForword_BC1,TIM_MOTOR_Z,MotorProtect);	// 正转1秒
					OS_AddJudegeFunction(taskMotor,MotorBACK_BC1,TIM_MOTOR_F,MotorProtect);		// 反转1.5秒
					OS_AddFunction(taskMotor,MotorSTOP,4);					// 停止
					OS_AddFunction(taskMotor,WindowStateBC1,4);				// 关窗标志置位
					motorStruct.flag_BC1 = 0;	
					motorStruct.hasrun = 0;
					if(shut_time != 0)			//计算关窗用的时间
					{
						shut_time = GetSysTime(&timer2) - shut_time;
						FLASH_ProgramWord(ADDR_shut_time,shut_time);	// FLASH保存本次关窗的时间
						shut_time = 0;
						//没有FLYS时，启动定时器计时，延时打开窗
						flag_YS_isno = 1;
							
					}
					jugeYS.switchon = 0;		//不进行Y30按键检测
					flag_flag_FL_SHUT = 0;
				}
				if(motorStruct.flag_BC2)
				{
					debug("开窗限位\r\n");
					OS_AddJudegeFunction(taskMotor,MotorForword_BC2,TIM_MOTOR_Z,MotorProtect);	// 正转1秒
					OS_AddJudegeFunction(taskMotor,MotorBACK_BC2,TIM_MOTOR_F,MotorProtect);		// 反转1.5秒
					OS_AddFunction(taskMotor,MotorSTOP,4);					// 停止
					OS_AddFunction(taskMotor,WindowStateBC2,4);				// 关窗标志置位
					motorStruct.flag_BC2 = 0;	
					motorStruct.hasrun = dm_counter;
				}
				
				OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
				OS_AddTask(tasklink,taskMotor);								// 添加到任务队列
			}		
			//YS达到阀值，关窗
			else if(jugeYS.switchon && flag_flag_FL_SHUT == 0)
			{
				flag_flag_FL_SHUT = 1;
				debug("\r\nYS达到阀值，关窗\r\n");
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddJudegeFunction(taskMotor,ShutDownWindow,60000,YSFL_MotorProtect);	// 执行关窗
				OS_AddFunction(taskMotor,MotorSTOP,4);							// 停止
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
				if(key_AM.val == on)
					shut_time = GetSysTime(&timer2);							//AM下自动记录关窗时间
			}else
			if(flag_FL_SHUT)//FL达到阀值，关窗
			{
				debug("FL达到阀值，关窗\r\n");
				flag_FL_SHUT = 0;
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddJudegeFunction(taskMotor,ShutDownWindow,60000,YSFL_MotorProtect);	// 执行关窗
				OS_AddFunction(taskMotor,MotorSTOP,4);							// 停止
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务				
				OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
	//			if(key_AM.val == on)
	//				shut_time = GetSysTime(&timer2);							//AM下自动记录关窗时间			
			}else
				if(jugeYS_No.switchon)											//无YS开窗
				{
					debug("无YS开窗\r\n");
					jugeYS_No.switchon = 0;
					OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
					OS_AddJudegeFunction(taskMotor,OpenWindow,FLASH_ReadWord(ADDR_shut_time),MotorProtect);	// 执行开窗
					OS_AddFunction(taskMotor,MotorSTOP,4);			// 停止
					OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
					OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
				}
			//按键<Z
			if(flag_KEY_Z)
			{
				flag_KEY_Z = 0;
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				switch(key_Z.val)
				{
					case off:OS_AddFunction(taskMotor,MotorSTOP,4);				// STOP关窗
					break;
					case on:OS_AddJudegeFunction(taskMotor,Motor_Z,4,MotorProtect);				// 执行<z
					break;
					case two:	//关1/3
						debug("flag_KEY_Z :two\r\n");
						if(motorStruct.hasrun > dm_counter*2/3)
							OS_AddJudegeFunction(taskMotor,Motor_Z,motorStruct.hasrun - dm_counter*2/3,MotorProtect);	// 执行<z
						else OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter*2/3 - motorStruct.hasrun,MotorProtect);// 执行<z
						OS_AddFunction(taskMotor,MotorSTOP,4);
					break;
					case three:// 关2/3
						debug("flag_KEY_Z :three\r\n");
						if(motorStruct.hasrun > dm_counter/3)
							OS_AddJudegeFunction(taskMotor,Motor_Z,motorStruct.hasrun - dm_counter/3,MotorProtect);	// 执行<z
						else OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter/3 - motorStruct.hasrun,MotorProtect);// 执行<z
						OS_AddFunction(taskMotor,MotorSTOP,4);
					break;
				}
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
				OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
			}else

			//按键>Y
			if(flag_KEY_Y)
			{
				flag_KEY_Y = 0;
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				switch(key_Y.val)
				{
					case off:OS_AddFunction(taskMotor,MotorSTOP,4);				// STOP开窗
					break;
					case on:OS_AddJudegeFunction(taskMotor,Motor_Y,4,MotorProtect);				// 执行>Y
					break;
					case two:// 开1/3
					debug("KEY_Y :two\r\n");
					debug("motorStruct.hasrun =0x%x%x\r\n",(u16)(motorStruct.hasrun>>16),(u16)(motorStruct.hasrun));
						if(motorStruct.hasrun > dm_counter/3)
							OS_AddJudegeFunction(taskMotor,Motor_Z,motorStruct.hasrun - dm_counter/3,MotorProtect);// 执行>Y
						else OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter/3 - motorStruct.hasrun,MotorProtect);	
						OS_AddFunction(taskMotor,MotorSTOP,4);
					break;
					case three:// 开2/3
						debug("KEY_Y :three\r\n");
						if(motorStruct.hasrun > dm_counter*2/3)
							OS_AddJudegeFunction(taskMotor,Motor_Z,motorStruct.hasrun - dm_counter*2/3,MotorProtect);	// 执行>Y
						else OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter*2/3 - motorStruct.hasrun,MotorProtect);		
						OS_AddFunction(taskMotor,MotorSTOP,4);
					break;
				}
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
				OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
			}	
		}		
	}
	MX830Motor_StateDir(&motorStruct);	 								// 马达根据命令运动
}

//BC1,BC2
WindowState CheckWindowState()
{
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	if(GPIO_READ(GPIO_38KHZ_BC1) == RESET)windowstate = to_BC1;
	else if(GPIO_READ(GPIO_38KHZ_BC2) == RESET)windowstate = to_BC2;
	else windowstate = open;
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_Out_PP_Low_Slow);
	return windowstate;
}
void CheckBC1BC2()
{
	
	if(motorStruct.dir != STOP)	
	{	
		CheckWindowState();
		if(motorStruct.dir == FORWARD && windowstate == to_BC1 && motorStruct.flag_BC == 0 && flag_KEY_Z != 1) motorStruct.flag_BC1 = 1;
		if(motorStruct.dir == BACK &&  windowstate == to_BC2 && motorStruct.flag_BC == 0 && flag_KEY_Y != 1) motorStruct.flag_BC2 = 1;	
	}

	if((windowstate == to_BC1 && motorStruct.dir == FORWARD) ||(windowstate == to_BC2 && motorStruct.dir == BACK))
		windowstate = open;
	if(motorStruct.flag_BC && OsJudge_TaskIsNull(taskMotor)) motorStruct.flag_BC = 0;	// 没有任务

}














