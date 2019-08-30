#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "CUI_RTOS.H"

u8 flag_exti = 0;
u8 key_val = 0;
keyStr key_Z = {0};
keyStr key_Y= {0};
keyStr key_AM = {0};
keyStr key_Y30 = {0};

extern	TimerLinkStr 	timer2 ;					// 任务的定时器

// 松手程序
void Key_ScanLeave()
{    						
    if((GPIO_READ(GPIO_DER_Z)&GPIO_READ(GPIO_DER_Y)&GPIO_READ(GPIO_AM)&GPIO_READ(GPIO_Y30)))
    {       
	  	debug("key null\r\n");
        flag_exti = 0;
		key_val = 0;
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
	  
	  GPIO_Init(GPIO_DER_Z,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_DER_Y,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_AM,	KEY_GPIOMODE);
	  GPIO_Init(GPIO_Y30,	KEY_GPIOMODE);
		
	  disableInterrupts();
      EXTI_SelectPort(EXTI_Port_B);
      EXTI_SetHalfPortSelection(EXTI_HalfPort_B_LSB,ENABLE);   
      EXTI_SetPortSensitivity(EXTI_Port_B,EXTI_Trigger_Falling);				// 设置端口敏感性
	  //使能中断
      enableInterrupts();                                                 // 使能中断
}

INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	debug("EXTI1_IRQHandler\r\n");
	EXTI_ClearITPendingBit (EXTI_IT_Pin1);
	
}

//按键扫描
void ScanKey(keyStr* key)
{
	LSI_delayms(2);
	if(GPIO_ReadInputDataBit(key->GPIOx,key->GPIO_Pin) == RESET)
	{
		u32 time = GetSysTime(&timer2);
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
			ScanKey(&key_Y30);
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

