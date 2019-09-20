#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "CUI_RTOS.H"
#include "LED_SHOW.H"
#include "ADC_CHECK.H"
#include "stmflash.h"
//#include "MX830Motor.h"

u8 		flag_exti	= 0;
u8 		key_val 	= 0;
keyStr 	key_Z 		= {0};
keyStr 	key_Y		= {0};
keyStr 	key_AM 		= {0};
keyStr 	key_Y30 	= {0};
keyStr 	key_DM 		= {0};

u8 		flag_duima  		= 0;	//对码状态
u8 		flag_duima_clear  	= 0;	//清除对码
u32 	Y30_Risingtime 		= 0;
u32 	DM_Risingtime 		= 0;		

u8 					flag_KEY_Z 	= 0;				// 传递给马达函数，让他根据val做出动作
u8 					flag_KEY_Y 	= 0;
u8					signal_key = 0;

extern	TimerLinkStr 	timer2 ;					// 任务的定时器
extern	JugeCStr 		YS_30 ;
extern	TaskStr* 		taskKeyScan;
extern	u16				amtime ;
extern	u16				y30time ;
extern	JugeCStr 		LEDAM_juge ;
extern	JugeCStr 		LEDY30_juge ;
extern	JugeCStr 		beep;
extern 	u8				flag_motorIO;
extern 	TaskLinkStr* 	tasklink;


void BeepStart()
{
	beep.start = 1;
	#if BEEP_SW > 0
	GPIO_SET(GPIO_BEEP);
	#else
	GPIO_RESET(GPIO_BEEP);
	#endif
	
}

// 松手程序
void Key_ScanLeave()
{    	
	
	if(Y30_Risingtime != 0 && GPIO_READ(GPIO_Y30))
	{
		if((GetSysTime(&timer2) - Y30_Risingtime) > TIM_Y30_DELAY)
		{
			debug("取消Y30延时");
			YS_30.start = 0;
			YS_30.counter = 0;//取消Y30延时
		}
		Y30_Risingtime = 0;
	}

	if(DM_Risingtime != 0 && GPIO_READ(GPIO_DM))
	{
		if((GetSysTime(&timer2) - DM_Risingtime) > TIM_DML && (GetSysTime(&timer2) - DM_Risingtime) < TIM_DMH )
		{
			 flag_duima = 1;	//对码状态
		}
		else if((GetSysTime(&timer2) - DM_Risingtime) > TIM_DM_CLEARL && (GetSysTime(&timer2) - DM_Risingtime) < TIM_DM_CLEARH )
		{
			flag_duima_clear = 1;	//清除对码
		}
		DM_Risingtime = 0;
	}
	
    if(GPIO_READ(GPIO_DER_Z)&&GPIO_READ(GPIO_DER_Y)&&GPIO_READ(GPIO_AM)&&GPIO_READ(GPIO_Y30)&&GPIO_READ(GPIO_DM))
    {
	 //	debug("-----key null-------\r\n");
        flag_exti = 0;
	//	key_val = KEY_VAL_NULL;
    }
}

void keyStr_Init(keyStr* key,GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef GPIO_Pin)
{
	key->GPIOx = GPIOx;
	key->GPIO_Pin = GPIO_Pin;
	key->counter = 0;
	key->val = off;
}
void Key_GPIO_Init()
{
	  keyStr_Init(&key_Z,GPIO_DER_Z);
	  keyStr_Init(&key_Y,GPIO_DER_Y);
	  keyStr_Init(&key_AM,GPIO_AM);
	  keyStr_Init(&key_Y30,GPIO_Y30);
	  keyStr_Init(&key_DM,GPIO_DM);
	  
	  GPIO_Init(GPIO_DER_Z,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_DER_Y,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_AM,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_Y30,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_DM,	KEY_GPIOMODE);
	  
	  disableInterrupts();
      EXTI_SelectPort(EXTI_Port_B);
      EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB,ENABLE);   
      EXTI_SetPortSensitivity(EXTI_Port_B,EXTI_Trigger_Falling);				// 设置端口敏感性
	  EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
	  //使能中断
      enableInterrupts();                                                 		// 使能中断
}




//按键扫描
u32 ScanKey(keyStr* key)
{
	//LSI_delayms(2);
	u32 time = 0;
	if(GPIO_ReadInputDataBit(key->GPIOx,key->GPIO_Pin) == RESET)
	{
		time = GetSysTime(&timer2);
		if(key->counter == 0)
		{
			key->counter = time;
			key->val = on;
		}	
		else
		{
			if(time < (key->counter + TIM_KEY))	//连击
			{
				if(key->val < six)
					key->val ++;
				key->counter = time;
			}else 
			{
				key->val = off;	//停止
				key->counter = 0;
				
			}
		}	
			
	}
				
	return time;	
}

//按键处理函数
void KeyFun()
{					
	if(key_val)
	{
		switch(key_val)
		{
			case KEY_VAL_DER_Z:	flag_KEY_Z = 1;
				break;
			case KEY_VAL_DER_Y:	flag_KEY_Y = 1;
				break;
			case KEY_VAL_AM: 	
				if(key_AM.val == off)	amtime = TIM_AM_OFF;	// 对应的LED指示点亮0.5秒后熄灭
				else					amtime = TIM_AM_ON;		// 对应的LED指示点亮30秒后熄灭
				LEN_GREEN_Open();
				LEDAM_juge.start = 1;
				LEDAM_juge.counter = 0;
				break;
			case KEY_VAL_Y30:

				if(jugeYS.start || jugeYS.switchon)	//有YS
				{
					switch(key_Y30.val)
					{
						case 1: ys_timer30 = TIM_30; 		YS_30.start = 1;	break;
						case 2: ys_timer30 = TIM_30 * 2; 	YS_30.start = 1;	break;
						case 3: ys_timer30 = TIM_30 * 6; 	YS_30.start = 1;	break;
					}
					YS_30.start = 1;
					YS_30.counter = 0;
					y30time = TIM_Y30_ON;
					debug("ys_timer30 = %d\r\n",ys_timer30);
				}else y30time = TIM_Y30_OFF;
				LEN_RED_Open();
				LEDY30_juge.start = 1;
				LEDY30_juge.counter = 0;
				break;
			case KEY_VAL_DM:	
				//debug("key_DM.val = %d\r\n",key_DM.val);
				if(key_DM.val == six)	//对话马达转向
				{
						flag_motorIO = ~flag_motorIO;
						FLASH_ProgramByte(ADDR_motorIO,flag_motorIO);
						key_DM.val = off;
						debug("马达对换引脚\r\n");
				}
		}
		
		
		BeepStart();		
		key_val = KEY_VAL_NULL;
	}
}

bool Juge_key()
{
	return (bool)signal_key;
}
void KeyScanControl()
{
	if(flag_exti && taskKeyScan->state == Stop)
	{
		signal_key = 0;
		OS_AddJudegeFunction(taskKeyScan,OS_DeleteTask,TIM_KEY,Juge_key);			
		OS_AddJudegeFunction(taskKeyScan,KeyFun,4,Juge_key);
		OS_AddFunction(taskKeyScan,OS_DeleteTask,0);	
		OS_AddTask(tasklink,taskKeyScan);// 添加检测任务
	}
}

INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
//	debug("in key ti,flag_exti = %d\r\n",flag_exti);
   // if(key_val == KEY_VAL_NULL)
    {
		if(GPIO_READ(GPIO_DER_Z) == RESET)
		{
			ScanKey(&key_Z);
			key_val = KEY_VAL_DER_Z;
		}else
		if(GPIO_READ(GPIO_DER_Y) == RESET)
		{
			ScanKey(&key_Y);
			key_val = KEY_VAL_DER_Y;
		}else
		if(GPIO_READ(GPIO_AM) == RESET)
		{
			ScanKey(&key_AM);
			key_val = KEY_VAL_AM;
		}else
		if(GPIO_READ(GPIO_Y30) == RESET)
		{
			Y30_Risingtime = ScanKey(&key_Y30);		//保存按下的时刻
			key_val = KEY_VAL_Y30;
		}
		if(key_val)
		{
			flag_exti = 1;   
			signal_key = 1;
		//	debug("key_val = %d\r\n",key_val);
		}	
    }

     EXTI_ClearITPendingBit (EXTI_IT_PortB);  
       
}

INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
  //  if(key_val == KEY_VAL_NULL)
    {
		if(GPIO_READ(GPIO_DM) == RESET)
		{
			DM_Risingtime = ScanKey(&key_DM);
			key_val = KEY_VAL_DM;
		}
		if(key_val)
		{
			flag_exti = 1; 
			signal_key = 1;
			//debug("key_val = %d\r\n",key_val);
		}	
    }
//	debug("EXTI7_IRQHandler\r\n");
	EXTI_ClearITPendingBit (EXTI_IT_Pin7);
}
