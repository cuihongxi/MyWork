#include "keyboard.h"
#include "stm8l15x_exti.h"
#include "CUI_RTOS.H"
#include "LED_SHOW.H"
#include "ADC_CHECK.H"
#include "stmflash.h"
#include "24l01.h"
#include "MX830Motor.h"

u8 			flag_exti	= 0;
u8 			key_val 	= 0;
keyStr 		key_Z 		= {0};
keyStr 		key_Y		= {0};
keyStr 		key_AM 		= {0};
keyStr 		key_Y30 	= {0};
keyStr 		key_DM 		= {0};

u8 			flag_duima  		= 0;	//对码状态
u8 			flag_duima_clear  	= 0;	//清除对码
u32 		Y30_Risingtime 		= 0;
u32 		DM_Risingtime 		= 0;		
u32 		AM_Risingtime 		= 0;

u8 		flag_KEY_Z 		= 0;	// 传递给马达函数，让他根据val做出动作
u8 		flag_KEY_Y 		= 0;
u8		flag_I30_en		= 0;	// 开窗报警使能

extern	TimerLinkStr 		timer2 ;	// 任务的定时器
extern	JugeCStr 			YS_30 ;
extern	TaskStr* 			taskKeyScan;
extern	u16					amtime ;

extern	JugeCStr 			LEDAM_juge ;
extern	JugeCStr 			LEDY30_juge ;
extern	JugeCStr 			beep;
extern 	u8					flag_DM;
extern 	TaskLinkStr* 		tasklink;
extern 	u8 			ledSharpTimes;
extern 	bool		is_suc;
extern 	u32 		shut_time;
extern 	u32 		ystime;
extern 	u8 			beepTimes;
extern	u32			beepdelayon;
extern	u32			beepdelayoff;
extern	u8  		ADDRESS1[TX_ADR_WIDTH];
extern	u8  		ADDRESS2[RX_ADR_WIDTH];
extern	u8			flag_BH;
extern 	u8			flag_YS_isno;
extern 	u16			nrf_sleeptime 	;
extern 	u16			nrf_worktime	;
extern 	u16			led_ontime		;
extern 	u16			led_offtime		;
extern	u16 		YS_CGdat ;
extern 	u32 		systime;
extern	JugeCStr 	clearCGYS ;
extern	u8			flag_FLCheckState ;				//fl检测开始还是停止
extern	u8			flag_FL_SHUT;
extern	u8 			flag_1	;						// 进行YS检查的时候置一

// 清除DM
void ClearDM();

void BeepStart()
{
	beep.start = 1;
//	#if BEEP_SW > 0
//	GPIO_SET(GPIO_BEEP);
//	#else
	GPIO_RESET(GPIO_BEEP);
//	#endif
	
}
//成功状态：LED闪烁6次，蜂鸣器同步
void StateSuccess()
{
		ledSharpTimes = 12;
		led_ontime = TIM_LED_SHARP_ON;
		led_offtime = TIM_LED_SHARP_OFF;
		is_suc = (bool)TRUE;
		beepTimes = 2;
		beepdelayoff = 200;
		beepdelayon = 100;
}

//取消状态：LED闪烁6次，蜂鸣器同步
void StateFalse()
{
		ledSharpTimes = 12;
		led_ontime = TIM_LED_SHARP_ON;
		led_offtime = TIM_LED_SHARP_OFF;
		beepTimes = 2;
		beepdelayon = 500;
		beepdelayoff = 10;
		is_suc = (bool)FALSE;
}

//AM切换
void SwitchAM()
{
		if(key_AM.val == off)
		{
			key_AM.val = on;
			debug("打开AM延时\r\n");
			StateSuccess();
		}
		else 
		{
			key_AM.val = off;
			flag_YS_isno = 0;
			jugeYS_No.start = 0;
			jugeYS_No.counter =0;
			jugeYS_No.switchon = 0;
			shut_time = 0;
			debug("取消AM\r\n");
			StateFalse();
		}
		AM_Risingtime = 0;
		ledSharpTimes = 12;
		led_ontime = TIM_LED_SHARP_ON;
		led_offtime = TIM_LED_SHARP_OFF;
}
// 指示当前AM状态
void ShowAMState()
{
  	debug("ShowAMState\r\n");
	ledSharpTimes = 2;
	led_ontime = SHOW_STATE_TIME;
	led_offtime = TIM_LED_SHARP_OFF;
	if(key_AM.val == off)is_suc = (bool)FALSE;
	else is_suc = (bool)TRUE;
}


// Y30切换
void SwitchY30()
{
	if(key_Y30.val == off)
	{
		key_Y30.val = on;			
		YS_30.start = 1;
		YS_30.counter = 0;
		ys_timer30 = TIM_30;
		Y30_Risingtime = 0;
		StateSuccess();
		debug("ys_timer30 = %d\r\n",ys_timer30);

	}
	else 
	{
			key_Y30.val = off;
		debug("取消Y30延时");
		YS_30.start = 0;
		YS_30.counter = 0;//取消Y30延时
		YS_30.switchon = 0;
		ys_timer30 = 0;
 		StateFalse();
		key_val = KEY_VAL_NULL;
		Y30_Risingtime = 0;
	}
}

// 指示当前Y30状态
void ShowY30State()
{
	ledSharpTimes = 2;
	led_ontime = SHOW_STATE_TIME;
	led_offtime = 100;
	if(key_Y30.val == off)is_suc = (bool)FALSE;
	else is_suc = (bool)TRUE;
}




// 松手程序
void Key_ScanLeave()
{    	
    if(Y30_Risingtime != 0)
    {
		if(GPIO_READ(GPIO_Y30) ==RESET)
		{
			if((systime - Y30_Risingtime) > TIM_Y30_DELAY) SwitchY30();
		}else 
		{
		  	if((systime - Y30_Risingtime) < TIM_Y30_DELAY) ShowY30State();		//LED显示当前Y30状态
			Y30_Risingtime = 0;
		}
    }
    
    if(AM_Risingtime != 0)
    {
		if(GPIO_READ(GPIO_AM) == RESET)
		{
			if((systime - AM_Risingtime) > TIM_Y30_DELAY) SwitchAM();
		}
		else 
		{
		  
		  if((systime - AM_Risingtime) < TIM_Y30_DELAY) ShowAMState();			//LED显示当前AM状态;
			AM_Risingtime = 0;
			
		}
    }
     
	if(DM_Risingtime != 0 && GPIO_READ(GPIO_DM))
	{
	    	debug("systime = %lu\r\n",systime);
		if((systime - DM_Risingtime) > TIM_DML && (systime - DM_Risingtime) < TIM_DMH )
		{
			flag_duima = 1;	//对码状态
			StateSuccess();
			debug("对码状态\r\n");
		}
		else if((systime - DM_Risingtime) > TIM_DM_CLEARL && (systime - DM_Risingtime) < TIM_DM_CLEARH )
		{
			flag_duima_clear = 1;	//清除对码
			StateFalse();
			// 清除DM
 			ClearDM();
			debug("清除对码\r\n");
		}
		DM_Risingtime = 0;
	}
	
    if(GPIO_READ(GPIO_DER_Z)&&GPIO_READ(GPIO_DER_Y)&&GPIO_READ(GPIO_AM)&&GPIO_READ(GPIO_Y30)&&GPIO_READ(GPIO_DM))
    {
	 	//debug("-----key null-------\r\n");
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
	EXTI_SetPortSensitivity(EXTI_Port_B,EXTI_Trigger_Falling);	// 设置端口敏感性
#ifndef DM_LED_RESET        
	EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Falling);
#else
        EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
#endif
	//使能中断
	enableInterrupts();                                                 // 使能中断
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
			if(key == &key_Z)key_Y.counter = 0;
			if(key == &key_Y)key_Z.counter = 0;
			debug("key on\r\n");
		}	
		else
		{
			if(time < (key->counter + TIM_KEY))	//连击
			{
			    debug("连击 = %d\r\n",key->val);
				if(key->val < six)
					key->val ++;
				key->counter = time;
			}else 
			{
				key->val = off;	//停止
				key->counter = 0;
				debug("key off\r\n");
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
		    case KEY_VAL_DER_Z:	
		    if((flag_DM == 0 && windowstate != SHUTDOWN) || (flag_DM != 0 && windowstate != OPENDOWN))flag_KEY_Z = 1;
                      else key_Z.counter = 0;
		    BeepStart();
				break;
		    case KEY_VAL_DER_Y:
		    if((flag_DM == 0 && windowstate != OPENDOWN) || (flag_DM != 0 && windowstate !=SHUTDOWN) )flag_KEY_Y = 1;
                    else key_Y.counter = 0;
		    BeepStart();
				break;
			case KEY_VAL_DM:	BeepStart();
				if(key_DM.val == six)	//对话马达转向
				{
						flag_DM = ~flag_DM;
						FLASH_ProgramByte(ADDR_DM,flag_DM);
						key_DM.val = off;
						debug("马达对换引脚\r\n");
						ledSharpTimes = 12;
						led_ontime = TIM_LED_SHARP_ON;
						led_offtime = TIM_LED_SHARP_OFF;
						is_suc = (bool)TRUE;
						beepTimes = 2;
						beepdelayoff = 200;
						beepdelayon = 100;
				}
		}
				
		key_val = KEY_VAL_NULL;
	}
}

void ChangeNRFCmd(u8* buf)
{
  switch(buf[5])
  {
    case CMD_AM:			
			if(buf[6] == MES_AM) SwitchAM();
			else if(buf[6] == MES_AM_CHECK) ShowAMState();
    break;
      case CMD_Y30:
			if(buf[6] == MES_Y30_CLEAR)
			{
				key_Y30.val = off;
				debug("取消Y30延时");
				YS_30.start = 0;
				YS_30.counter = 0;//取消Y30延时
				YS_30.switchon = 0;
				ys_timer30 = 0;
				StateFalse();
				key_val = KEY_VAL_NULL;
				Y30_Risingtime = 0;
			}else
			 if(buf[6] == MES_Y30_3_1)
			 {
			 	key_Y30.val = on;			
				YS_30.start = 1;
				YS_30.counter = 0;
				ys_timer30 = TIM_30;
				Y30_Risingtime = 0;
				ledSharpTimes = 2;
				led_ontime = TIM_LED_SHARP_ON;
				led_offtime = TIM_LED_SHARP_OFF;
				is_suc = (bool)TRUE;
				beepTimes = 2;
				beepdelayoff = 200;
				beepdelayon = 100;
				debug("ys_timer30 = %d\r\n",ys_timer30);
			 }else
			 if(buf[6] == MES_Y30_3_2)
			 {
			 	key_Y30.val = on;			
				YS_30.start = 1;
				YS_30.counter = 0;
				ledSharpTimes = 4;
				led_ontime = TIM_LED_SHARP_ON;
				led_offtime = TIM_LED_SHARP_OFF;
				is_suc = (bool)TRUE;
				ys_timer30 = TIM_30*4;
				Y30_Risingtime = 0;
				beepTimes = 4;
				beepdelayoff = 200;
				beepdelayon = 100;
				debug("ys_timer30 = %d\r\n",ys_timer30);
			 }else
			 if(buf[6] == MES_Y30_3_3)
			 {
			 	key_Y30.val = on;			
				YS_30.start = 1;
				YS_30.counter = 0;
				ledSharpTimes = 6;
				led_ontime = TIM_LED_SHARP_ON;
				led_offtime = TIM_LED_SHARP_OFF;
				is_suc = (bool)TRUE;
				ys_timer30 = TIM_30*8;
				Y30_Risingtime = 0;
				beepTimes = 6;
				beepdelayoff = 200;
				beepdelayon = 100;
				debug("ys_timer30 = %d\r\n",ys_timer30);
			 }else
			 if(buf[6] == MES_Y30_CHECK) ShowY30State();
        
    break;
      case CMD_Z:
			key_val = KEY_VAL_DER_Z;
			
			if(buf[6] == MES_Z)
			{
			  	if(key_Z.counter == 0)
				{
				  	key_Z.counter = GetSysTime(&timer2);
					key_Z.val = on;
					key_Y.counter = 0;
					debug("key on\r\n");
				}else
				{
					key_Z.val = off;	//停止
					key_Z.counter = 0;
					debug("key off\r\n");
				}

			}else
			{	
			  	key_Z.counter = GetSysTime(&timer2);
				key_Y.counter = 0;
				if(buf[6] == MES_Z_3_1) key_Z.val = two;
				else if(buf[6] == MES_Z_3_2) key_Z.val = three;
				else if(buf[6] == MES_Z_3_3) key_Z.val = four;
			} 

    break;
      case CMD_Y:
			key_val = KEY_VAL_DER_Y;
			if(buf[6] == MES_Y)
			{
			  	if(key_Y.counter == 0)
				{
				  	key_Y.counter = GetSysTime(&timer2);
					key_Y.val = on;
					key_Z.counter = 0;
					debug("key on\r\n");
				}else
				{
					key_Y.val = off;	//停止
					key_Y.counter = 0;
					debug("key off\r\n");
				}

			}else
			{	
			  	key_Y.counter = GetSysTime(&timer2);
				key_Z.counter = 0;
				if(buf[6] == MES_Y_3_1) key_Y.val = two;
				else if(buf[6] == MES_Y_3_2) key_Y.val = three;
				else if(buf[6] == MES_Y_3_3) key_Y.val = four;
			} 		
		
    break;
      case CMD_I30:
		if(buf[6] == MES_I30_ALARM)
		{
			flag_I30_en = ~flag_I30_en;
			debug("flag_I30_en = %d\r\n",flag_I30_en);
		}
    break;
      case CMD_WAKE:
		if(buf[6] == MES_WAKE_UP)
		{
		  	debug("WAKE_UP~~\r\n");
		}else
		  if(buf[6] == MES_WAKE_SLEEP)
		{
		  		debug("WAKE_UP~~~\r\n");
		}
    break;    
	    case CMD_CG:
			if(flag_1) 
			{
				YS_CGdat = *(u16*)buf;
				debug("------>YS_CGdat = %d\r\n",YS_CGdat);
			}
			if(flag_FLCheckState) 
			{
				flag_FL_SHUT = buf[2];
				debug("------>CMD_FL : %d,%d\r\n",flag_FL_SHUT,buf[2]);
			}
			
    break;   
	
  }
}

INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
	debug("\r\nin key ti	");
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
		    	AM_Risingtime = systime;
			key_val = KEY_VAL_AM;
		}else
		if(GPIO_READ(GPIO_Y30) == RESET)
		{
			Y30_Risingtime = systime;
			debug("Y30_Risingtime = %lu\r\n",Y30_Risingtime);
			key_val = KEY_VAL_Y30;
		}
		if(key_val)
		{
			flag_exti = 1;   
		}	
    }

     EXTI_ClearITPendingBit (EXTI_IT_PortB);  
       
}

#ifndef DM_LED_RESET
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
    if(key_val == KEY_VAL_NULL)
    {
		if(GPIO_READ(GPIO_DM) == RESET)
		{
			DM_Risingtime = ScanKey(&key_DM);
			key_val = KEY_VAL_DM;
		}
		debug("--KEY_VAL_DM\r\n");
		if(key_val)flag_exti = 1; 	
    }
	EXTI_ClearITPendingBit (EXTI_IT_Pin1);
	
}
#else

void BH_Check();

INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    if(key_val == KEY_VAL_NULL)
    {
		if(GPIO_READ(GPIO_DM) == RESET)
		{
			DM_Risingtime = ScanKey(&key_DM);
			key_val = KEY_VAL_DM;
			debug("KEY_VAL_DM\r\n");
		}
		if(key_val)flag_exti = 1; 
	
    }
    
    BH_Check();//BH检测
    EXTI_ClearITPendingBit (EXTI_IT_Pin7);
}


#endif