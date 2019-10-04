#include "main.h"

Nrf24l01_PRXStr 	prx 		= {0};				// NRF���սṹ��
u8 			txbuf[] 	= {1};				// nrf���ͻ���
u8 			rxbuf[10] 	= {0};				// nrf���ջ���
u8 			flag_wake 	= 1;				// ���ѱ�־
u8 			flag_openDM = 0;				// ����DM��־

u32 			dm_counter 	= 0;				// �������DM��������
TimerLinkStr 		timer2 		= {0};				// ����Ķ�ʱ��
TaskLinkStr		task_link 	= {0};
TaskLinkStr* 		tasklink 	= &task_link;			// �����б�
u16 			sleeptime	= TIM_SLEEP;			// ˯�ߵ���ʱ

u32 			threshold_30 	= 0;				// 30�����޶�����ֵ
BATStr 			bat = {0};					// ��ؽṹ��
TaskStr* 		taskBatControl 	= {0};	
TaskStr* 		taskYS			= {0};			// YS��������
TaskStr* 		taskKeyScan		= {0};			// KEY ɨ��
TaskStr* 		taskInMain   	={0};


JugeCStr 		beep = {0};

JugeCStr 		NRFpowon = {0};
JugeCStr 		NRFpowoff = {0};

u16			amtime = 0;
u32 			systime = 0;
u8 			ledSharpTimes = 0;
bool			is_suc = (bool)FALSE;
u8 			beepTimes = 0;
u32 			beepdelayon = 0;
u32 			beepdelayoff = 0;
extern u32 		counter_BH;					// BH����
extern u8 		flag_no30;
extern keyStr 		key_AM;
extern keyStr 		key_Y30;
extern keyStr 		key_DM;
extern u8 		flag_duima  		;	//����״̬
extern u8 		flag_duima_clear  	;	//�������

//�������ݳ�ʼ��
//void Wake_InitDat()
//{
//	flag_checkBat = 1;			//ÿ�λ��ѡ����BAT����һ��
//}
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
	GPIO_Init(GPIO_BEEP,GPIO_Mode_Out_PP_High_Slow);
}



//TIM2��ʼ��
//void TIM2_INIT()
//{
//    enableInterrupts();                                                 //ʹ��ȫ���ж�
//    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);              //��ʱ��
//    TIM2_DeInit();
//    TIM2_TimeBaseInit(TIM2_Prescaler_1,TIM2_CounterMode_Up,2000);         //1ms 
//    TIM2_ARRPreloadConfig(ENABLE);
//    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
//    TIM2_ClearITPendingBit(TIM2_IT_Update); 
//    TIM2_Cmd(ENABLE);
//
//}

//��ϵͳ����
void Make_SysSleep()
{
//	debug("sleep:\r\n");
	NRF24L01_PWR(0); 
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// �ر�SPIʱ��
	CLK_LSICmd(ENABLE);						//ʹ��LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	//RTCʱ��ԴLSI��
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	//�ȴ�LSI����
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	//RTCʱ���ſ�ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		//19K
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	//�����ж�
	RTC_SetWakeUpCounter(19);                     			//���Ѽ��	1MS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	//���Ѷ�ʱ���ж�ʹ��
	RTC_WakeUpCmd(ENABLE);                                      	//RTC����ʹ��  
	PWR_UltraLowPowerCmd(ENABLE); 					//ʹ�ܵ�Դ�ĵ͹���ģʽ
	PWR_FastWakeUpCmd(ENABLE);
	
	//debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
	flag_wake = 0;
}

//��ϵͳ����
void MakeSysWakeUp()
{
	//TIM2_Cmd(ENABLE);
	sleeptime = TIM_SLEEP;
	debug(" WakeUp \r\n");
}

void BeepStart();
void BeepStop()
{
//	#if BEEP_SW > 0
//	GPIO_RESET(GPIO_BEEP);
//	#else
	GPIO_SET(GPIO_BEEP);
//	#endif
}

void FunInMain()
{	
	CheckBC1BC2();		// �����λ
	YS_Control();		// YS���������
	FL_Control();		// FL���������
	MotorControl();		// ����˶�
	BatControl(&bat,tasklink,taskBatControl);	// ��Դ����
	
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

//����������
void KeyFun();
void main()
{

  	CLK_GPIO_Init();					// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	//delay_ms(1000);					// �ȴ�ϵͳ�ȶ�
	UART_INIT(115200);
	prx.txbuf[0] =39; 					//�յ��ظ���Ϣ����䡮9��
	LEN_RED_Close();
	LEN_GREEN_Close();
	FlashData_Init();
	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskMotor = OS_CreatTask(&timer2);			// ���������������
	taskYS = OS_CreatTask(&timer2);				// ����YS�������� ��ÿ2����һ��
	taskKeyScan = OS_CreatTask(&timer2);			// ��������ɨ������
	taskInMain = OS_CreatTask(&timer2);			// ������������������
	OS_AddFunction(taskInMain,FunInMain,40);	
	OS_AddTask(tasklink,taskInMain);
	Make_SysSleep();					// ϵͳ��������״̬
	FL_GPIO_Init();
	//���һ�ε�ص�ѹ
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	debug("bat = %d.%d\r\n",(u8)bat.val,(u8)(bat.val*10)-(u8)bat.val*10);	
	//�ϵ���DM��ƽ�����ж���������г�ʱ��	
	if(GPIO_READ(GPIO_DM) == RESET)
	{
		
		//��ﷴת����λ
		motorStruct.command = FORWARD;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC1) != RESET);	// �ȴ�GPIO_38KHZ_BC1���ֵ͵�ƽ
		flag_openDM = 1;
		motorStruct.hasrun = 0;
		motorStruct.command = BACK;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC2) != RESET);	// �ȴ�GPIO_38KHZ_BC2���ֵ͵�ƽ
		flag_openDM = 0;
		dm_counter = motorStruct.hasrun;
		FLASH_ProgramWord(ADDR_DM,dm_counter);		// д��FLASH
		FLASH_ProgramByte(ADDR_flag_BH,flag_BH);	// д��FLASH
		debug("dm_counter = %lu\r\n",dm_counter);
	}
	
	CheckWindowState();							// ��һ�´���λ��
	Key_GPIO_Init();							// ����������ʼ��
	
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	NRFpowon.start = 1;
	while(1)
	{
      if(flag_wake)
	  { 
	  }
	  else	//���ߺ���
	  {            
            	halt();
			 
		if(flag_wake == 0)
		{	
			//KeyScanControl();	// ����ɨ��
			 KeyFun();
			
			OS_Task_Run(tasklink);
			if(flag_exti)	Key_ScanLeave();            // ���ֳ���
			if(flag_duima)		//����״̬
			{
				
			}
			/*nrf���պ���*/
			if(prx.hasrxlen != 0)
			{
			  	debug("hasrxlen = %d :\r\n",prx.hasrxlen);		
				for(u8 i=0;i<prx.hasrxlen;i++)
				  {
					debug("rxbuf[%d]=%d	",i,prx.rxbuf[i]);
				  }
				
				debug("\r\n##################################\r\n");
				if(prx.rxbuf[2] == 95) debug("taskMotor->state = %d\r\n",taskMotor->state);
				prx.hasrxlen = 0;
			}	
		}else
			MakeSysWakeUp();
	  }

	}
}

//IRQ �жϷ�����
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
		
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) 
	{
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);	
		prx.IRQCallBack(&prx);
		CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);	
	}
//	flag_wake = 1;
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
	
	 systime = OS_TimerFunc(&timer2);			// ��ʱ���ں���
	
	//��ص������
	if(systime >= bat.threshold) bat.flag = 1;
	// ������м�ʱ
	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)	
	{
		motorStruct.counter += IRQ_PERIOD;		// ������г�����ֵ��ͣת
		counter_BH += IRQ_PERIOD;			// BH������ֵû�д�����ͣת
		if(motorStruct.counter > MOTOR_F_SAFE) motorStruct.erro |= ERROR_MOTOR;
		if(counter_BH > BH_SAFE) motorStruct.erro |= ERROR_BH;
	}
	
	//YS
	Juge_counter(&jugeYS,TIM__YS_D);

	//��YS��ʱ����
	Juge_counter(&jugeYS_No,TIM_OPEN);
	//YS 30���Ӳ���Ӧ
	if(Juge_counter(&YS_30, (u32)60000*ys_timer30))
	{
	    debug("Y30 stop windowstate = %d\r\n",windowstate);
		if(flag_no30) 
		{
			flag_no30 = 0; 	// ��λ��BH�޷����������ʱ30����
//			CheckWindowState();
//			if(windowstate != to_BC1) FL_CheckStart();
		}
	}
	//BEEP
	if(Juge_counter(&beep,130)) BeepStop();
	//���óɹ�ʱ����
	LedSharpInIT(&ledSharpTimes,is_suc,systime,100,500);
	BeepInIT(&beepTimes,systime,beepdelayon,beepdelayoff);
	 
	//nrf����򿪵�Դ
	if(Juge_counter(&NRFpowon,40)) 
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
//TIM2�����ж�,1ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
 // TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}


