#include "main.h"
#include "CUI_RTOS.H"
Nrf24l01_PRXStr 	prx 		= {0};				// NRF接收结构体
u8 			txbuf[5] 	= {0};				// nrf发送缓存
u8 			rxbuf[10] 	= {0};				// nrf接收缓存
u8 			flag_openDM 	= 0;				// 开机DM标志
u32 			dm_counter 	= 0;				// 开机检测DM，计数器

TimerLinkStr 		timer2 		= {0};				// 任务的定时器
TaskLinkStr		task_link 	= {0};
TaskLinkStr* 		tasklink 	= &task_link;			// 任务列表

BATStr 			bat = {0};					// 电池结构体
TaskStr* 		taskBatControl 	= {0};	
TaskStr* 		taskYS		= {0};				// YS测量任务
TaskStr* 		taskKeyScan	= {0};				// KEY 扫描
TaskStr* 		taskInMain   	={0};

JugeCStr 		beep 		= {0};
JugeCStr 		NRFpowon 	= {0};
JugeCStr 		NRFpowoff 	= {0};

//u16			amtime 		= 0;
u32 			systime 	= 0;				// 保存系统时间，ms
u8 			ledSharpTimes 	= 0;				// 控制LED闪烁次数
bool			is_suc 		= (bool)FALSE;			// 设置是否成功
u8 			beepTimes 	= 0;				// 蜂鸣器响的次数
u32 			beepdelayon 	= 0;				// 蜂鸣器响的时间
u32 			beepdelayoff 	= 0;				// 蜂鸣器关闭的时间

extern u32 		counter_BH;			// BH计数
extern u8 		flag_no30;
extern keyStr 		key_AM;
extern keyStr 		key_Y30;
extern keyStr 		key_DM;
extern u8 		flag_duima  		;	//对码状态
extern u8 		flag_duima_clear  	;	//清除对码
void Motor_Forward();
//低时钟和GPIO初始化
void CLK_GPIO_Init()
{
	CLK_Change2HSI();				//切换HSI时钟
	GPIO_Init(GPIO_DM,GPIO_Mode_In_PU_No_IT);
	LED_GPIO_Init();   				// 双色LED初始化
	MX830Motor_GPIOInit();                      	// 马达IO配置	
	GPIO_ADC_Init();
   	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_DM,	GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_BEEP,GPIO_Mode_Out_PP_High_Slow);
}


//让系统休眠
void Make_SysSleep()
{
	NRF24L01_PWR(0); 						// 关闭NRF的电源
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// 关闭SPI时钟
	CLK_LSICmd(ENABLE);						// 使能LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTC时钟源LSI，
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// 等待LSI就绪
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTC时钟门控使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19K时钟频率
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 开启中断
	RTC_SetWakeUpCounter(10);                     			// 唤醒间隔	1MS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 唤醒定时器中断使能
	RTC_WakeUpCmd(ENABLE);                                      	// RTC唤醒使能  
	PWR_UltraLowPowerCmd(ENABLE); 					// 使能电源的低功耗模式
	PWR_FastWakeUpCmd(ENABLE);
	
}


void BeepStart();
void BeepStop()
{
	GPIO_SET(GPIO_BEEP);
}

void FunInMain()
{	
	CheckBC1BC2();		// 检测限位
	YS_Control();		// YS，供电控制
	FL_Control();		// FL，供电控制
	MotorControl();		// 马达运动
	BatControl(&bat,tasklink,taskBatControl);	// 电源管理
	
}

void BeepInIT(u8* time,u32 systime,u32 ontime,u32 offtime)
{
   	static u32 sys = 0; 
	if(*time)
	{
	    if(systime > sys)
	    {
		    if(*time & 0x01)
		    {
			GPIO_SET(GPIO_BEEP);
			sys = systime + offtime;
		    }
		    else 
		    {
			GPIO_RESET(GPIO_BEEP);
			sys = systime + ontime;
		    }
		    (*time) --;
	    }	    	     
	    if(*time == 0) {BeepStop();sys = 0;}
	}
}

//按键处理函数
void KeyFun();
void main()
{

  	CLK_GPIO_Init();					// 低功耗时钟和GPIO初始化,2MHZ
	//delay_ms(1000);					// 等待系统稳定
	UART_INIT(115200);
	prx.txbuf[0] =39; 					//收到回复信息，填充‘9’

	FlashData_Init();
	taskBatControl = OS_CreatTask(&timer2);			// 创建电池电量检测任务
	taskMotor = OS_CreatTask(&timer2);			// 创建马达运行任务
	taskYS = OS_CreatTask(&timer2);				// 创建YS测量任务 ，每2秒检测一次
	taskKeyScan = OS_CreatTask(&timer2);			// 创建按键扫描任务
	taskInMain = OS_CreatTask(&timer2);			// 创建主函数运行任务
	OS_AddFunction(taskInMain,FunInMain,40);	
	OS_AddTask(tasklink,taskInMain);
	Make_SysSleep();					// 系统进入休眠状态
	BH_FL_GPIO_Init();
	//检测一次电池电压
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	debug("bat = %d.%d\r\n",(u8)bat.val,(u8)(bat.val*10)-(u8)bat.val*10);
	
	//马达反转到限位
	motorStruct.command = FORWARD;
	MX830Motor_StateDir(&motorStruct);
	while(GPIO_READ(GPIO_38KHZ_BC1) != RESET);	// 等待GPIO_38KHZ_BC1出现低电平
		
	//上电检测DM电平，来判断马达的最大行程时间	
	if(GPIO_READ(GPIO_DM) == RESET)
	{
		flag_openDM = 1;
		motorStruct.hasrun = 0;
		motorStruct.command = BACK;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC2) != RESET);	// 等待GPIO_38KHZ_BC2出现低电平
		flag_openDM = 0;
		dm_counter = motorStruct.hasrun;
		FLASH_ProgramWord(ADDR_DM,dm_counter);		// 写入FLASH
		FLASH_ProgramByte(ADDR_flag_BH,flag_BH);	// 写入FLASH
		debug("dm_counter = %lu\r\n",dm_counter);
	}
	
	CheckWindowState();							// 读一下窗的位置
	Key_GPIO_Init();							// 触摸按键初始化	
	
	while(1)
	{         
            	halt(); //停止模式		
		KeyFun();					// 按键处理
		
		OS_Task_Run(tasklink);				// 执行任务链表中的任务
		
		if(flag_exti)	Key_ScanLeave();            	// 松手程序
		
		

	}
}

//IRQ 中断服务函数
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
		
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) 
	{
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);	
		prx.IRQCallBack(&prx);
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);	
	}
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
	 systime = OS_TimerFunc(&timer2);			// OS定时器内函数，获得系统时间
	 
	if(systime >= bat.threshold) bat.flag = 1;		// 电池电量检测间隔
	
	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)// 马达运行计时	
	{
	    	static u8 ledsharp = 254;
		motorStruct.counter += IRQ_PERIOD;		// 马达运行超过阀值，停转
		counter_BH += IRQ_PERIOD;			// BH超过阀值没有触发，停转
		if(motorStruct.counter > MOTOR_F_SAFE) motorStruct.erro |= ERROR_MOTOR;
		if(counter_BH > BH_SAFE) motorStruct.erro |= ERROR_BH;
		LedSharpInIT(&ledsharp,(bool)TRUE,systime,TIM_MOTORLED_SHARP_ON,TIM_MOTORLED_SHARP_OFF);// LED闪烁控制
		if(ledsharp == 0)ledsharp = 254;
	}
	
	Juge_counter(&jugeYS,TIM__YS_D);			// YS
	Juge_counter(&jugeYS_No,TIM_OPEN);			// 无YS计时开窗
	
	if(Juge_counter(&YS_30, (u32)60000*ys_timer30))		//YS 30分钟不响应
	{
	    	debug("Y30 stop windowstate = %d\r\n",windowstate);
		if(flag_no30) flag_no30 = 0; 			// 复位由BH无方波引起的延时30分钟
	}
	
	if(Juge_counter(&beep,130)) BeepStop();			// 按键蜂鸣器BEEP
	LedSharpInIT(&ledSharpTimes,is_suc,systime,TIM_LED_SHARP_ON,TIM_LED_SHARP_OFF);	// 设置时，LED闪烁控制
	BeepInIT(&beepTimes,systime,beepdelayon,beepdelayoff);	// 设置时，beep控制
	 
	
	if(Juge_counter(&NRFpowon,40)) 				//nrf间隔打开电源,ms
	{
		//debug("NRFpowon\r\n");
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);	
		NRF24L01_PWR(1);
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);	
		NRFpowoff.start = 1;
	}
	if(Juge_counter(&NRFpowoff,40)) 
	{
	//	debug("NRFpowoff\r\n");
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);
		NRF24L01_PWR(0);
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);	
		NRFpowon.start = 1;
	}
	
   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}



