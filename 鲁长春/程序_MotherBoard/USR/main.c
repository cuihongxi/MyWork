#include "main.h"



TimerLinkStr 		timer2 		= {0};				// ����Ķ�ʱ��
TaskLinkStr		task_link 	= {0};
TaskLinkStr* 		tasklink 	= &task_link;		// �����б�

BATStr 			bat = {0};					        // ��ؽṹ��
TaskStr* 		taskBatControl 	= {0};	
TaskStr* 		taskYS		= {0};				    // YS��������
TaskStr* 		taskNRF	        = {0};				// NRF
TaskStr* 		taskInMain   	={0};
TaskStr* 		taskAlarm   	={0};

JugeCStr 		beep 		= {0};
JugeCStr 		NRFpowon 	= {0};
JugeCStr 		NRFpowoff 	= {0};
JugeCStr 		NRFsleep 	= {0};

u32 			systime 	= 0;				// ����ϵͳʱ�䣬ms
u8 			ledSharpTimes 	= 0;				// ����LED��˸����
bool			is_suc 		= (bool)FALSE;			// �����Ƿ�ɹ�
u8 			beepTimes 	= 0;				// ��������Ĵ���
u32 			beepdelayon 	= 0;				// ���������ʱ��
u32 			beepdelayoff 	= 0;				// �������رյ�ʱ��
u16				nrf_sleeptime 	= DJ_SLEEP_TIME;
u16				nrf_worktime	= DJ_WORK_TIME;
u16				led_ontime		= TIM_LED_SHARP_ON;
u16				led_offtime		= TIM_LED_SHARP_OFF;

extern u32 		counter_BH;			// BH����
extern u8 		flag_no30;
extern keyStr 		key_AM;
extern keyStr 		key_Y30;
extern keyStr 		key_DM;
extern u8 		flag_duima  		;	//����״̬
extern u8 		flag_duima_clear  	;	//�������
extern	JugeCStr	jugeWindows;
extern	JugeCStr	jugeBHLED;

void Motor_Forward();
//��ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
	CLK_Change2HSI();				//�л�HSIʱ��
	GPIO_Init(GPIO_DM,GPIO_Mode_In_PU_No_IT);
	LED_GPIO_Init();   				// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                      	// ���IO����	
	GPIO_ADC_Init();
   	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_DM,	GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_BEEP,GPIO_Mode_Out_OD_HiZ_Slow);
}


//��ϵͳ����
void Make_SysSleep()
{
	NRF24L01_PWR(0); 						// �ر�NRF�ĵ�Դ
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// �ر�SPIʱ��
	CLK_LSICmd(ENABLE);						// ʹ��LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTCʱ��ԴLSI��
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// �ȴ�LSI����
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTCʱ���ſ�ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19Kʱ��Ƶ��
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// �����ж�
	RTC_SetWakeUpCounter(380);                     			// ���Ѽ��	20MS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// ���Ѷ�ʱ���ж�ʹ��
	RTC_WakeUpCmd(ENABLE);                                      	// RTC����ʹ��  
	PWR_UltraLowPowerCmd(ENABLE); 					// ʹ�ܵ�Դ�ĵ͹���ģʽ
	PWR_FastWakeUpCmd(ENABLE);
	
}


void BeepStart();
void BeepStop()
{
	GPIO_SET(GPIO_BEEP);
}

void FunInMain()
{	
	CheckBC1BC2();		// �����λ
	YS_Control();		// YS���������
	FL_Control();		// FL���������
	MotorControl();		// ����˶�
	BatControl(&bat,tasklink,taskBatControl);	// ��Դ����
#if	USE_NRF > 0
        NRF_Function();
#endif
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
void FunInSleap()
{
	 systime = OS_TimerFunc(&timer2);			// OS��ʱ���ں��������ϵͳʱ��

	if(systime >= bat.threshold) bat.flag = 1;		// ��ص��������
	
	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)// ������м�ʱ	
	{
	    
		motorStruct.counter += IRQ_PERIOD;		// ������г�����ֵ��ͣת
		counter_BH += IRQ_PERIOD;			// BH������ֵû�д�����ͣת
		if(motorStruct.counter > MOTOR_F_SAFE) motorStruct.erro |= ERROR_MOTOR;
		if(counter_BH > BH_SAFE) motorStruct.erro |= ERROR_BH;

	}
	
	Juge_counter(&jugeYS,TIM__YS_D);			// YS
	Juge_counter(&jugeYS_No,TIM_OPEN);			// ��YS��ʱ����
	
	if(Juge_counter(&YS_30, (u32)60000*ys_timer30))		//YS 30���Ӳ���Ӧ
	{
	    	key_Y30.val = off;
		if(flag_no30) flag_no30 = 0; 			// ��λ��BH�޷����������ʱ30����
	}
	
	if(Juge_counter(&beep,130)) BeepStop();			// ����������BEEP
	LedSharpInIT(&ledSharpTimes,is_suc,systime,led_ontime,led_offtime);	// ����ʱ��LED��˸����
	BeepInIT(&beepTimes,systime,beepdelayon,beepdelayoff);	// ����ʱ��beep����
	
	if(Juge_counter(&jugeWindows,TIM__YS_D))
	{
	    GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_Out_PP_Low_Slow);
	    GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_Out_PP_Low_Slow);	
	}
	if(Juge_counter(&jugeBHLED,TIM__YS_D))    
	{
		LEN_GREEN_Close();
	}
#if	USE_NRF > 0	
	if(Juge_counter(&NRFpowon,nrf_sleeptime)) 				//nrf����򿪵�Դ,ms
	{
		NRF24L01_PWR(1);
		NRFpowoff.start = 1;
	}
	if(Juge_counter(&NRFpowoff,nrf_worktime)) 
	{
		NRF24L01_PWR(0);
		NRFpowon.start = 1;
	}
	
	if(Juge_counter(&NRFsleep,10000)) 
	{
	  debug("SLEEP\r\n");
		nrf_sleeptime = DJ_SLEEP_TIME;
		nrf_worktime = DJ_WORK_TIME;
	}	
#endif
}

//����������
void KeyFun();
void main()
{

  	CLK_GPIO_Init();					// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	//delay_ms(1000);					// �ȴ�ϵͳ�ȶ�
	UART_INIT(115200);
	FlashData_Init();

	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskMotor = OS_CreatTask(&timer2);			// ���������������
	taskYS = OS_CreatTask(&timer2);				// ����YS�������� ��ÿ2����һ��
	taskNRF = OS_CreatTask(&timer2);			// ����nrf����
	taskAlarm =  OS_CreatTask(&timer2);	
	taskInMain = OS_CreatTask(&timer2);			// ������������������
	
	OS_AddFunction(taskInMain,FunInMain,40);	
	OS_AddTask(tasklink,taskInMain);
	Make_SysSleep();					// ϵͳ��������״̬
	BH_FL_GPIO_Init();
	//���һ�ε�ص�ѹ
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	debug("bat = %d.%d\r\n",(u8)bat.val,(u8)(bat.val*10)-(u8)bat.val*10);


#if	USE_NRF > 0
	NRF_CreatNewAddr(ADDRESS2);

#endif
	NRF24L01_PWR(0);
	CheckWindowState();
  	Key_GPIO_Init();							// ����������ʼ��	
	while(1)
	{         
        halt(); 					// ֹͣģʽ
		KeyFun();					// ��������
		if(flag_exti)	Key_ScanLeave();            	// ���ֳ���
		FunInSleap();

		OS_Task_Run(tasklink);				// ִ�����������е�����
			
		if(jugeBHLED.start && GPIO_READ(GPIO_BH)){jugeBHLED.start = 0;LEN_GREEN_Close();} 
		if(jugeWindows.start && (motorStruct.dir == STOP||motorStruct.dir == NULL))
		{
		    	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
			GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
			if(GPIO_READ(GPIO_38KHZ_BC1) == RESET||GPIO_READ(GPIO_38KHZ_BC2) == RESET)
			{
			    jugeWindows.start = 0;ledSharpTimes = 2;is_suc = (bool)FALSE;
			    GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_Out_PP_Low_Slow);
			    GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_Out_PP_Low_Slow);
			}
		}
		if( GPIO_READ(CHARGE_PRO_PIN) != RESET)		//���״̬
		{
		    while( GPIO_READ(CHARGE_PRO_PIN) != RESET)
		    {
		    	float val = BatteryGetAD(Get_BAT_ADC_Dat(Battery_Channel));
				if(val >= VALVE_BAT_CHARGE){LEN_RED_Close();LEN_GREEN_Open();}
				else {LEN_GREEN_Close();LEN_RED_Open();}
				delay_ms(100);		    
		    }
		   WWDG_SWReset();	// ��λ
		}
    
 
	}
}


//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    // IWDG_ReloadCounter();
   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}



