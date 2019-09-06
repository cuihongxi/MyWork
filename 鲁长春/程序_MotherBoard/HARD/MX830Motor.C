#include "UHEAD.H"
#include "MX830Motor.h"
#include "ADC_CHECK.H"
#include "lowpower.h"
#include "keyboard.h"
#include "FL.H"
#include "stmflash.h"
#include "BAT.H"

Motor_Struct    		motorStruct = {0};      // 马达状态结构体
TaskStr* 				taskMotor;				// 马达运动任务
u32 					shut_time = 0;
u8						flag_motorIO = 0;		// 马达引脚调换标志
u8						flag_YS_isno = 0;		// YS无检测

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
		motorStruct->command = MOTOR_NULL;
		if(motorStruct->dir != motorStruct->command)	//换向
		{
			
			motorStruct->dir = motorStruct->command;
			motorStruct->counter = 0;
		}
		switch(motorStruct->dir)
		{
			case FORWARD    :     
					debug("dir = FORWARD\r\n");           
					Motor_Forward();
			  		BH_CheckStart();
			  break;
			case BACK       :        
			debug("dir = BACK\r\n"); 
					Motor_Back();
					BH_CheckStart();
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

//
bool MotorProtect()
{
	return (bool)(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) || GPIO_READ(CHARGE_PRO_PIN) != RESET || counter_BH > BH_SAFE);
}

//马达运动
void MotorControl()
{
	//充电状态禁止转动，超过转动时限，BAT电压过低禁止转动
	if(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) || GPIO_READ(CHARGE_PRO_PIN) != RESET || flag_no30 == 1 || bat.state == BAT_STATE_NoBACK)
	{
		motorStruct.command = STOP;	//正反转保护时间判断
		motorStruct.flag_BC1 = 0;
		motorStruct.flag_BC2 = 0;
	}
	else
	//所有情况下马达正转至38BC1高电平，延时1秒（可调）后反转1.5秒（可调）停转；\
	反转至38BC2高电平，延时1秒正转1.5秒停转。
	if((taskMotor->state == Wait || taskMotor->state == Stop))
	{
		if(counter_BH > BH_SAFE && flag_30 == 0)							// 方波超时保护
		{
			flag_30 = 1;													//如果出现BH，则会在中断中清零
			counter_BH = 0;
			//按马达当前转向，反向旋转4S，再继续原来转向
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			OS_AddJudegeFunction(taskMotor,BH_Motor1,TIM_BH0,MotorProtect);	// 反向旋转4S
			OS_AddJudegeFunction(taskMotor,BH_Motor1,10,MotorProtect);		//恢复原来的转向
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			
		}else if(counter_BH > BH_SAFE && flag_30 == 1)						//再次出现无方波
		{
			motorStruct.command = STOP;	//正反转保护时间判断
			//30分钟不响应YS信号
			flag_no30 = 1;
			threshold_30 = GetSysTime(&timer2) + TIM_30;
			flag_30 = 0;
			counter_BH = 0;
		}else
		if((motorStruct.flag_BC1||motorStruct.flag_BC2) && motorStruct.flag_BC == 0)
		{
			motorStruct.flag_BC = 1;
			OS_AddFunction(taskMotor,OS_DeleteTask,0);				// 移除任务
			if(motorStruct.flag_BC1)								// 关窗限位
			{
				OS_AddJudegeFunction(taskMotor,MotorForword_BC1,TIM_MOTOR_Z,MotorProtect);	// 正转1秒
				OS_AddJudegeFunction(taskMotor,MotorBACK_BC1,TIM_MOTOR_F,MotorProtect);		// 反转1.5秒
				OS_AddFunction(taskMotor,MotorSTOP,4);				// 停止
				motorStruct.flag_BC1 = 0;	
				if(shut_time != 0)			//计算关窗用的时间
				{
					shut_time = GetSysTime(&timer2) - shut_time;
					FLASH_ProgramWord(ADDR_shut_time,shut_time);	// FLASH保存本次关窗的时间
					shut_time = 0;
					//没有FLYS时，启动定时器计时，延时打开窗
					flag_YS_isno = 1;
						
				}
			}
			if(motorStruct.flag_BC2)
			{
				OS_AddJudegeFunction(taskMotor,MotorForword_BC2,TIM_MOTOR_Z,MotorProtect);	// 正转1秒
				OS_AddJudegeFunction(taskMotor,MotorBACK_BC2,TIM_MOTOR_F,MotorProtect);		// 反转1.5秒
				OS_AddFunction(taskMotor,MotorSTOP,4);				// 停止
				motorStruct.flag_BC2 = 0;			
			}
			OS_AddFunction(taskMotor,OS_DeleteTask,0);				// 移除任务
		}else
		
		//YSFL达到阀值，关窗
		if(jugeYS.switchon)
		{
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			OS_AddJudegeFunction(taskMotor,ShutDownWindow,4,MotorProtect);	// 执行关窗
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
			jugeYS.switchon = 0;
			if(key_AM.val == on)
				shut_time = GetSysTime(&timer2);							//AM下自动记录关窗时间
		}else
		if(flag_FL_SHUT)//FL达到阀值，关窗
		{
			flag_FL_SHUT = 0;
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			OS_AddJudegeFunction(taskMotor,ShutDownWindow,4,MotorProtect);	// 执行关窗
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
//			if(key_AM.val == on)
//				shut_time = GetSysTime(&timer2);							//AM下自动记录关窗时间			
		}else
			if(jugeYS_No.switchon)											//无YS开窗
			{
				jugeYS_No.switchon = 0;
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
				OS_AddJudegeFunction(taskMotor,OpenWindow,FLASH_ReadWord(ADDR_shut_time),MotorProtect);	// 执行开窗
				OS_AddJudegeFunction(taskMotor,MotorSTOP,4,MotorProtect);		// 停止
				OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
			}
		//按键<Z
		if(flag_KEY_Z)
		{
			flag_KEY_Z = 0;
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			switch(key_Z.val)
			{
				case off:OS_AddFunction(taskMotor,MotorSTOP,4);				// 执行关窗
				break;
				case on:OS_AddJudegeFunction(taskMotor,Motor_Z,4,MotorProtect);				// 执行<z
				break;
				case two:
					OS_AddJudegeFunction(taskMotor,Motor_Z,dm_counter/3,MotorProtect);			// 执行<z
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
				case three:
					OS_AddJudegeFunction(taskMotor,Motor_Z,dm_counter*2/3,MotorProtect);		// 执行<z
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
			}
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
		}else

		//按键>Y
		if(flag_KEY_Y)
		{
			flag_KEY_Y = 0;
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			switch(key_Y.val)
			{
				case off:OS_AddFunction(taskMotor,MotorSTOP,4);				// 执行关窗
				break;
				case on:OS_AddJudegeFunction(taskMotor,Motor_Y,4,MotorProtect);				// 执行>Y
				break;
				case two:
					OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter/3,MotorProtect);			// 执行>Y
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
				case three:
					OS_AddJudegeFunction(taskMotor,Motor_Y,dm_counter*2/3,MotorProtect);		// 执行>Y
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
			}
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
		}
		
		OS_AddTask(tasklink,taskMotor);									// 添加到任务队列
	}
	
	MX830Motor_StateDir(&motorStruct);	 								// 马达根据命令运动
}

//BC1,BC2
void CheckBC1BC2()
{
	if(motorStruct.dir == FORWARD && GPIO_READ(GPIO_38KHZ_BC1) && motorStruct.flag_BC == 0) motorStruct.flag_BC1 = 1;
	if(motorStruct.dir == BACK && GPIO_READ(GPIO_38KHZ_BC2) && motorStruct.flag_BC == 0) motorStruct.flag_BC2 = 1;
	funLinkStr* pthis =  (funLinkStr*)&taskMotor->funNode;
	if(SingleList_Iterator((void**)&pthis) == 0)					//没有任务
		motorStruct.flag_BC = 0;

}














