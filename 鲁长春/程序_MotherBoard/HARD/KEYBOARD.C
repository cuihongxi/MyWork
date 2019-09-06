#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "CUI_RTOS.H"

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

extern	TimerLinkStr 	timer2 ;					// 任务的定时器
extern	JugeCStr 		YS_30 ;

// 松手程序
void Key_ScanLeave()
{    					
	if(Y30_Risingtime != 0 && GPIO_READ(GPIO_Y30))
	{
		if((GetSysTime(&timer2) - Y30_Risingtime) > TIM_Y30_DELAY)
		{
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
	
    if((GPIO_READ(GPIO_DER_Z)&GPIO_READ(GPIO_DER_Y)&GPIO_READ(GPIO_AM)&GPIO_READ(GPIO_Y30)))
    {       
	  	debug("key null\r\n");
        flag_exti = 0;
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
      enableInterrupts();                                                 // 使能中断
}




//按键扫描
u32 ScanKey(keyStr* key)
{
	LSI_delayms(2);
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
INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
	debug("in key ti\r\n");
    if(key_val == KEY_VAL_NULL)
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
			debug("key_val = %d\r\n",key_val);
		}	
    }

     EXTI_ClearITPendingBit (EXTI_IT_PortB);  
       
}

INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    if(key_val == KEY_VAL_NULL)
    {
		if(GPIO_READ(GPIO_DM) == RESET)
		{
			DM_Risingtime = ScanKey(&key_DM);
			key_val = KEY_VAL_DM;
		}
		if(key_val)
		{
			flag_exti = 1;   
			debug("key_val = %d\r\n",key_val);
		}	
    }
	debug("EXTI7_IRQHandler\r\n");
	EXTI_ClearITPendingBit (EXTI_IT_Pin7);
}
