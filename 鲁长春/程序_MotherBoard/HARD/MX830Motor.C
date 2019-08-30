#include "UHEAD.H"
#include "MX830Motor.h"
#include "ADC_CHECK.H"
#include "lowpower.h"
#include "keyboard.h"
#include "FL.H"

Motor_Struct    motorStruct = {0};      // 马达状态结构体
TaskStr* 		taskMotor;				// 马达运动任务


extern	u8 flag_YS_SHUT;
extern	TaskLinkStr* tasklink;			// 任务列表
extern	u8 flag_KEY_Z ;					// 传递给马达函数，让他根据val做出动作
extern	u8 flag_KEY_Y ;
extern	keyStr key_Z ;
extern	keyStr key_Y;
extern	u32 dm_counter;					// 开机检测DM，计数器
extern	u8 	flag_30 ;
extern u32 	counter_BH	;		// BH计数

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
			  GPIO_SetBits(MX830Motor_GPIO_FI);
			  GPIO_ResetBits(MX830Motor_GPIO_BI);
				BH_CheckStart();
			  break;
			case BACK       :        
			debug("dir = BACK\r\n"); 
					GPIO_ResetBits(MX830Motor_GPIO_FI);
					GPIO_SetBits(MX830Motor_GPIO_BI);

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


//马达运动
void MotorControl()
{
	//充电状态禁止转动，超过转动时限，禁止转动
	if(motorStruct.counter > ((u32)1000 * MOTOR_F_SAFE) || GPIO_READ(CHARGE_PRO_PIN) != RESET || flag_no30)
	{
		motorStruct.command = STOP;	//正反转保护时间判断
		motorStruct.flag_BC1 = 0;
		motorStruct.flag_BC2 = 0;
	}else if(counter_BH > BH_SAFE && flag_30 == 0)
	{
		flag_30 = 1;
		//按马达当前转向，反向旋转4S，再继续原来转向
		OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
		OS_AddFunction(taskMotor,BH_Motor1,TIM_BH0);				// 反向旋转4S
		OS_AddFunction(taskMotor,BH_Motor1,10);						//恢复原来的转向
		
	}else if(counter_BH > BH_SAFE && flag_30 == 1)					//再次出现无方波
	{
		motorStruct.command = STOP;	//正反转保护时间判断
		//30分钟不响应YS信号
		flag_no30 = 1;
		flag_30 = 0;
		counter_BH = 0;
	}
	else
	//所有情况下马达正转至38BC1高电平，延时1秒（可调）后反转1.5秒（可调）停转；\
	反转至38BC2高电平，延时1秒正转1.5秒停转。
	if((taskMotor->state == Wait || taskMotor->state == Stop))
	{
		if(motorStruct.flag_BC1||motorStruct.flag_BC2)
		{
			motorStruct.flag_BC = 1;
			if(motorStruct.flag_BC1)
			{
				OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
				OS_AddFunction(taskMotor,MotorForword_BC1,TIM_MOTOR_Z);		// 正转1秒
				OS_AddFunction(taskMotor,MotorBACK_BC1,TIM_MOTOR_F);		// 反转1.5秒
				OS_AddFunction(taskMotor,MotorSTOP,4);						// 停止
				OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
				motorStruct.flag_BC1 = 0;	
			}
			if(motorStruct.flag_BC2)
			{
				OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
				OS_AddFunction(taskMotor,MotorForword_BC2,TIM_MOTOR_Z);		// 正转1秒
				OS_AddFunction(taskMotor,MotorBACK_BC2,TIM_MOTOR_F);		// 反转1.5秒
				OS_AddFunction(taskMotor,MotorSTOP,4);						// 停止
				OS_AddFunction(taskMotor,OS_DeleteTask,0);					// 移除任务
				motorStruct.flag_BC2 = 0;			
			}		
		}
		
			//YSFL达到阀值，关窗
		if(flag_YS_SHUT)
		{
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			OS_AddFunction(taskMotor,Motor_Z,4);							// 执行关窗
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
			flag_YS_SHUT = 0;
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
				case on:OS_AddFunction(taskMotor,Motor_Z,4);				// 执行关窗
				break;
				case two:
					OS_AddFunction(taskMotor,Motor_Z,dm_counter/3);			// 执行关窗
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
				case three:
					OS_AddFunction(taskMotor,Motor_Z,dm_counter*2/3);		// 执行关窗
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
			}
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务	
		}

		//按键>Y
		if(flag_KEY_Y)
		{
			flag_KEY_Y = 0;
			OS_AddFunction(taskMotor,OS_DeleteTask,0);						// 移除任务
			switch(key_Y.val)
			{
				case off:OS_AddFunction(taskMotor,MotorSTOP,4);				// 执行关窗
				break;
				case on:OS_AddFunction(taskMotor,Motor_Y,4);				// 执行关窗
				break;
				case two:
					OS_AddFunction(taskMotor,Motor_Y,dm_counter/3);			// 执行关窗
					OS_AddFunction(taskMotor,MotorSTOP,4);
				break;
				case three:
					OS_AddFunction(taskMotor,Motor_Y,dm_counter*2/3);		// 执行关窗
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














