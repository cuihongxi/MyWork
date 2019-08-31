#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
//#include "stm8l15x_clk.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "lowpower.h"
#include "stm8l15x_rtc.h"
#include "stm8l15x_tim2.h"
#include "CUI_RTOS.H"
#include "keyboard.h"
#include "BAT.H"
#include "FL.H"

Nrf24l01_PRXStr prx = {0};		// NRF接收结构体
u8 txbuf[] = {1};				// nrf发送缓存
u8 rxbuf[10] = {0};				// nrf接收缓存

u8 flag_wake = 1;				// 唤醒标志
u8 flag_DM = 0;					// 开机检测DM，启动定时器标志
u32 dm_counter = 0;				// 开机检测DM，计数器

TimerLinkStr timer2 = {0};		// 任务的定时器
TaskLinkStr* tasklink = {0};			// 任务列表

u16 sleeptime = TIM_SLEEP;		// 睡眠倒计时

u8 flag_KEY_Z = 0;				//传递给马达函数，让他根据val做出动作
u8 flag_KEY_Y = 0;

extern u32 	counter_BH	;		// BH计数

extern u8 	flag_no30;
u32 threshold_30 = 0;			//30分钟无动作阀值

BATStr bat = {0};						// 电池结构体
TaskStr* taskBatControl;

//唤醒数据初始化
void Wake_InitDat()
{
	flag_checkBat = 1;			//每次唤醒。检测BAT电量一次
	
}
//低时钟和GPIO初始化
void CLK_GPIO_Init()
{
	CLK_Change2HSI();							//切换HSI时钟
	GPIO_Init(GPIO_DM,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	LED_GPIO_Init();   										// 双色LED初始化
	MX830Motor_GPIOInit();                                 	// 马达IO配置	
	GPIO_ADC_Init();
	
}



//TIM2初始化
void TIM2_INIT()
{
    enableInterrupts();                                                 //使能全局中断
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);              //打开时钟
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_Prescaler_1,TIM2_CounterMode_Up,2000);         //1ms 
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    TIM2_ClearITPendingBit(TIM2_IT_Update); 
    TIM2_Cmd(ENABLE);

}

//让系统休眠
void Make_SysSleep()
{
//	debug("sleep:\r\n");
//	LowPowerSet();
//	TIM2_Cmd(DISABLE);
	CLK_LSICmd(ENABLE);											//使能LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTC时钟源LSI，
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //等待LSI就绪
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTC时钟门控使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		//19K
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //开启中断
	RTC_SetWakeUpCounter(19);                     				//唤醒间隔	1MS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //唤醒定时器中断使能
	RTC_WakeUpCmd(ENABLE);                                      //RTC唤醒使能  
	//debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
	flag_wake = 0;
}

//让系统唤醒
void MakeSysWakeUp()
{

	//TIM2_Cmd(ENABLE);
	sleeptime = TIM_SLEEP;
	debug(" WakeUp \r\n");
}
void main()
{
  	CLK_GPIO_Init();										// 低功耗时钟和GPIO初始化,2MHZ
	delay_ms(500);											//等待系统稳定
	UART_INIT(115200);
//	debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
//	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_RED_Open(0);	
	Make_SysSleep();								// 系统进入休眠状态
	Key_GPIO_Init();								// 触摸按键初始化
	tasklink = SingleCycList_Create();
	
	taskBatControl = OS_CreatTask(&timer2);			// 创建电池电量检测任务
	taskMotor = OS_CreatTask(&timer2);				// 创建马达运行任务	
	TaskStr* taskYS = OS_CreatTask(&timer2);		// 创建YS测量任务 ，每2秒检测一次


	//上电检测DM电平，来判断马达的最大行程时间		
	if(GPIO_READ(GPIO_DM) == RESET)
	{
		//马达反转到限位
		motorStruct.command = FORWARD;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC1) != RESET);	//等待GPIO_38KHZ_BC1出现低电平
		flag_DM = 1;
		motorStruct.command = BACK;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC2) != RESET);	//等待GPIO_38KHZ_BC2出现低电平
		flag_DM = 0;
		debug("dm_counter = %lu\r\n",dm_counter);
	}
	FL_GPIO_Init();
	
	//检测一次电池电压
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	
	while(1)
	{
      if(flag_wake)
	  {
//			//按键松手检测
//			if(flag_exti)	Key_ScanLeave();                   //松手程序
//			//按键处理函数
//			if(key_val)
//			{
//				switch(key_val)
//				{
//						case KEY_VAL_DER_Z:
//					break;
//						case KEY_VAL_DER_Y:
//					break;
//						case KEY_VAL_AM:
//					break;
//						case KEY_VAL_Y30: Y30_function();
//					break;
//				}
//				key_val = KEY_VAL_NULL;
//			}
//			
//			//电源管理
//			BatControl();
//			//马达运动
// 			MotorControl();
//			//检测限位
//			CheckBC1BC2();
//			//YS—D，供电控制
//			YS_Control();	
//		//	debug("\r\nwake \r\n");	
//			OS_Task_Run(tasklink);	  
	  }
	  else	//休眠函数
	  {
          
 //           DATA_Init();               
            halt();
			if(flag_wake == 0)
			{
				if(flag_exti)	Key_ScanLeave();                   //松手程序
				//按键处理函数
				if(key_val)
				{
					switch(key_val)
					{
							case KEY_VAL_DER_Z:flag_KEY_Z = 1;
						break;
							case KEY_VAL_DER_Y:flag_KEY_Y = 1;
						break;
							case KEY_VAL_AM:
						break;
							case KEY_VAL_Y30: //Y30_function();
						break;
					}
					key_val = KEY_VAL_NULL;
				}
				
				//电源管理
				BatControl(&bat,tasklink,taskBatControl);
				
				//马达运动
				MotorControl();
				//检测限位
				CheckBC1BC2();
				//YS—D，供电控制
				YS_Control();	
				OS_Task_Run(tasklink);


			}else
				MakeSysWakeUp();
	 
	  }

	}
	
//	while(1)
//	{
			/*nrf接收函数*/
//			if(prx.hasrxlen != 0)
//			{
//			  debug("hasrxlen = %d :\r\n",prx.hasrxlen);		
//				for(u8 i=0;i<prx.hasrxlen;i++)
//				  {
//					debug("rxbuf[%d]=%d	",i,prx.rxbuf[i]);
//				  }
//				
//				debug("\r\n##################################\r\n");
//				
//				prx.hasrxlen = 0;
//			}	  
//	}
}

//IRQ 中断服务函数
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) prx.IRQCallBack(&prx);
	flag_wake = 1;
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
	
	u32 systime = OS_TimerFunc(&timer2);						// 定时器内函数
	if(flag_DM)	dm_counter ++;									// 对码计时
	
	//电池电量检测
	if(systime >= bat.threshold) bat.flag = 1;
	// 马达运行计时
	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)	
	{
		motorStruct.counter ++;
		counter_BH ++;
	}
	
	//YS
	if(flag_YS)
	{
		counter_YS++;
		if(counter_YS > TIM__YS_D*1000)	flag_YS_SHUT = 1;
	}
	//30
	if(flag_no30 && (systime > threshold_30)) flag_no30 = 0;	

   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}
//TIM2更新中断,1ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	
//	// 对码计时
//	if(flag_DM)	dm_counter ++;
//	
//	// 马达运行计时
//	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)	motorStruct.counter ++;
//
//	//YS
//	if(flag_YS)
//	{
//		counter_YS++;
//		if(counter_YS > TIM__YS_D*1000)	flag_YS_SHUT = 1;
//	}
//	
//	OS_TimerFunc(&timer2);							// 定时器内函数
//	
//	//睡眠倒计时
//	sleeptime --;
//	if(sleeptime == 0) Make_SysSleep();
  	TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}






