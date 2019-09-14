
#include "main.h"

Nrf24l01_PRXStr 	prx 		= {0};				// NRF���սṹ��
u8 					txbuf[] 	= {1};				// nrf���ͻ���
u8 					rxbuf[10] 	= {0};				// nrf���ջ���
u8 					flag_wake 	= 1;				// ���ѱ�־
u8 					flag_DM 	= 0;				// �������DM��������ʱ����־
u32 				dm_counter 	= 0;				// �������DM��������
TimerLinkStr 		timer2 		= {0};				// ����Ķ�ʱ��
TaskLinkStr* 		tasklink 	= {0};				// �����б�
u16 				sleeptime	= TIM_SLEEP;		// ˯�ߵ���ʱ
u8 					flag_KEY_Z 	= 0;				// ���ݸ���ﺯ������������val��������
u8 					flag_KEY_Y 	= 0;
u32 				threshold_30 	= 0;			// 30�����޶�����ֵ
BATStr 				bat = {0};						// ��ؽṹ��
TaskStr* 			taskBatControl 	= {0};	
TaskStr* 			taskLED = {0};
TaskStr*			taskLEDYS30 	= {0};
TaskStr* 			taskYS			= {0};			// YS��������
TaskStr* 			taskBEEP		= {0};			// ����������
u8 					signal_AMJudge 	= 0;			//AM�����ź���
u8					key_AM_state = 0;
u8 					signal_Y30Judge 	= 0;		//Y30�����ź���


extern u32 			counter_BH	;				// BH����
extern u8 			flag_no30;
extern keyStr 		key_AM;
extern keyStr 		key_Y30;
extern u8			flag_YS_isno;
extern keyStr 		key_DM;
extern u8			flag_motorIO;

//�������ݳ�ʼ��
//void Wake_InitDat()
//{
//	flag_checkBat = 1;			//ÿ�λ��ѡ����BAT����һ��
//}
//��ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
	CLK_Change2HSI();							//�л�HSIʱ��
	GPIO_Init(GPIO_DM,GPIO_Mode_In_PU_No_IT);
	LED_GPIO_Init();   							// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                      // ���IO����	
	GPIO_ADC_Init();
    GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_DM,	GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_BEEP,GPIO_Mode_Out_PP_Low_Slow);
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
//	LowPowerSet();
//	TIM2_Cmd(DISABLE);
	CLK_LSICmd(ENABLE);											//ʹ��LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTCʱ��ԴLSI��
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //�ȴ�LSI����
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTCʱ���ſ�ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		//19K
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //�����ж�
	RTC_SetWakeUpCounter(19);                     				//���Ѽ��	1MS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //���Ѷ�ʱ���ж�ʹ��
	RTC_WakeUpCmd(ENABLE);                                      //RTC����ʹ��  
	PWR_UltraLowPowerCmd(ENABLE); 								//ʹ�ܵ�Դ�ĵ͹���ģʽ
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

bool key_AM_Juge()
{
	bool i = (bool)signal_AMJudge;
	if(signal_AMJudge) signal_AMJudge = 0;
	return i;
}

bool key_Y30_Juge()
{
	bool i = (bool)signal_Y30Judge;
	if(signal_Y30Judge) signal_Y30Judge = 0;
	return i;
}

void BeepStart()
{
	GPIO_SET(GPIO_BEEP);
}
void BeepStop()
{
	GPIO_RESET(GPIO_BEEP);
}
void main()
{

  	CLK_GPIO_Init();								// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	delay_ms(500);									// �ȴ�ϵͳ�ȶ�
	UART_INIT(115200);

//	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_RED_Open(0);
	FlashData_Init();
	tasklink = SingleCycList_Create();				// ����һ������ѭ������
	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskMotor = OS_CreatTask(&timer2);				// ���������������
	taskLED = OS_CreatTask(&timer2);				// ����LED��ʾ����
	taskLEDYS30 =  OS_CreatTask(&timer2);			// ����LEDYS30��ʾ����
	taskYS = OS_CreatTask(&timer2);					// ����YS�������� ��ÿ2����һ��
	taskBEEP = OS_CreatTask(&timer2);				// ����beep����
	Make_SysSleep();								// ϵͳ��������״̬
	FL_GPIO_Init();
	 enableInterrupts();                                                 		// ʹ���ж�
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
		dm_counter = GetSysTime(&timer2);
		motorStruct.command = BACK;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC2) != RESET);	// �ȴ�GPIO_38KHZ_BC2���ֵ͵�ƽ
		dm_counter = GetSysTime(&timer2) - dm_counter;
		FLASH_ProgramByte(ADDR_DM,dm_counter);		// д��FLASH
		debug("dm_counter =0x%x%x\r\n",(u16)(dm_counter>>16),(u16)(dm_counter));
	}
	
	CheckWindowState();								// ��һ�´���λ��
	Key_GPIO_Init();								// ����������ʼ��
	
	while(1)
	{
      if(flag_wake)
	  { 
	  }
	  else	//���ߺ���
	  {
          
 //           DATA_Init();               
            halt();
			if(flag_wake == 0)
			{	
				//����������
				if(key_val)
				{
					switch(key_val)
					{
						case KEY_VAL_DER_Z:	flag_KEY_Z = 1;
							break;
						case KEY_VAL_DER_Y:	flag_KEY_Y = 1;
							break;
						case KEY_VAL_AM: 	
							if(OsJudge_TaskIsNull(taskLED)) signal_AMJudge = 0;
							else signal_AMJudge = 1;
							OS_AddFunction(taskLED,OS_DeleteTask,0);			// �Ƴ�����
							key_AM_state = !key_AM_state;
							if(key_AM_state == 0)							// ��Ӧ��LEDָʾ����0.5���Ϩ��
							{
								OS_AddFunction(taskLED,LEN_GREEN_Close,40);
								OS_AddJudegeFunction(taskLED,LEN_GREEN_Open,TIM_AM_OFF,key_AM_Juge);
								OS_AddFunction(taskLED,LEN_GREEN_Close,4);
								OS_AddFunction(taskLED,OS_DeleteTask,0);			// �Ƴ�����
							}else											// ��Ӧ��LEDָʾ����30���Ϩ��
							{
								
								OS_AddFunction(taskLED,LEN_GREEN_Close,40);
								OS_AddJudegeFunction(taskLED,LEN_GREEN_Open,TIM_AM_ON,key_AM_Juge);
								OS_AddFunction(taskLED,LEN_GREEN_Close,4);	
								OS_AddFunction(taskLED,OS_DeleteTask,0);			// �Ƴ�����
							}

							OS_AddTask(tasklink,taskLED);							// ���LED����	
							break;
						case KEY_VAL_Y30:
							if(OsJudge_TaskIsNull(taskLEDYS30)) signal_Y30Judge = 0;
							else signal_Y30Judge = 1;
							OS_AddFunction(taskLEDYS30,OS_DeleteTask,0);			// �Ƴ�����
							if(1)//if(jugeYS.start || jugeYS.switchon)	//��YS
							{
								switch(key_Y30.val)
								{
									case 1: ys_timer30 = TIM_30; 		YS_30.start = 1;	break;
									case 2: ys_timer30 = TIM_30 * 2; 	YS_30.start = 1;	break;
									case 3: ys_timer30 = TIM_30 * 6; 	YS_30.start = 1;	break;
								}
								key_Y30.val = off;
								YS_30.counter = 0;
								OS_AddFunction(taskLEDYS30,LEN_RED_Close,40);
								OS_AddJudegeFunction(taskLEDYS30,LEN_RED_Open,TIM_Y30_ON,key_Y30_Juge);
								OS_AddFunction(taskLEDYS30,LEN_RED_Close,4);	
								OS_AddFunction(taskLEDYS30,OS_DeleteTask,0);			// �Ƴ�����
							}else
							{
								OS_AddFunction(taskLEDYS30,LEN_RED_Close,40);
								OS_AddJudegeFunction(taskLEDYS30,LEN_RED_Open,TIM_Y30_OFF,key_Y30_Juge);
								OS_AddFunction(taskLEDYS30,LEN_RED_Close,4);
								OS_AddFunction(taskLEDYS30,OS_DeleteTask,0);			// �Ƴ�����
							}
							OS_AddTask(tasklink,taskLEDYS30);						// ���LED����	
							break;
						case KEY_VAL_DM:								
							if(key_DM.val == six)	//�Ի����ת��
							{
									flag_motorIO = ~flag_motorIO;
									FLASH_ProgramByte(ADDR_motorIO,flag_motorIO);
									key_DM.val = off;
									debug("���Ի�����\r\n");
							}
					}
					OS_AddFunction(taskBEEP,BeepStart,200);
					OS_AddFunction(taskBEEP,BeepStop,4);
					OS_AddFunction(taskBEEP,OS_DeleteTask,0);			// �Ƴ�����		
					OS_AddTask(tasklink,taskBEEP);
					key_val = KEY_VAL_NULL;
				}
				
				if(flag_exti)	Key_ScanLeave();                   					//���ֳ���
				
				//��Դ����
				BatControl(&bat,tasklink,taskBatControl);
				//����˶�
				MotorControl();
				//�����λ
				CheckBC1BC2();
				//YS��D���������
				YS_Control();	
				OS_Task_Run(tasklink);
			}else
				MakeSysWakeUp();
	 
	  }

	}
	
//	while(1)
//	{
			/*nrf���պ���*/
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

//IRQ �жϷ�����
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) prx.IRQCallBack(&prx);
	flag_wake = 1;
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
	
	u32 systime = OS_TimerFunc(&timer2);						// ��ʱ���ں���
	//if(flag_DM)	dm_counter ++;									// �����ʱ
	
	//��ص������
	if(systime >= bat.threshold) bat.flag = 1;
	// ������м�ʱ
	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)	
	{
		motorStruct.counter += IRQ_PERIOD;
		//counter_BH += IRQ_PERIOD;
		if(motorStruct.dir == BACK)
		{
			motorStruct.hasrun += IRQ_PERIOD;
		}else motorStruct.hasrun -= IRQ_PERIOD;
	}
	
	//YS
	if(jugeYS.start)
	{
		jugeYS.counter += IRQ_PERIOD;
		if(jugeYS.counter > TIM__YS_D)
		{
			debug("jugeYS.switchon = 1 \r\n");
			jugeYS.switchon = 1;
			jugeYS.start = 0;
			jugeYS.counter = 0;
		}	
	}
	//30
	if(flag_no30 && (systime > threshold_30)) flag_no30 = 0;	

	//��YS��ʱ����
	if(jugeYS_No.start) 
	{
		jugeYS_No.counter += IRQ_PERIOD;
		if(jugeYS_No.counter > TIM_OPEN)
		{
			jugeYS_No.start = 0;
			jugeYS_No.switchon = 1;
		}
	}
	//YS 30���Ӳ���Ӧ
	if(YS_30.start)
	{
		YS_30.counter += IRQ_PERIOD;
		if(YS_30.counter > ys_timer30)
		{
			YS_30.start = 0;
			YS_30.counter = 0;
		}
	}
	
   	RTC_ClearITPendingBit(RTC_IT_WUT);  
	
}
//TIM2�����ж�,1ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	
//	// �����ʱ
//	if(flag_DM)	dm_counter ++;
//	
//	// ������м�ʱ
//	if(motorStruct.dir == FORWARD || motorStruct.dir == BACK)	motorStruct.counter ++;
//
//	//YS
//	if(flag_YS)
//	{
//		counter_YS++;
//		if(counter_YS > TIM__YS_D*1000)	flag_YS_SHUT = 1;
//	}
//	
//	OS_TimerFunc(&timer2);							// ��ʱ���ں���
//	
//	//˯�ߵ���ʱ
//	sleeptime --;
//	if(sleeptime == 0) Make_SysSleep();
  	TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}






