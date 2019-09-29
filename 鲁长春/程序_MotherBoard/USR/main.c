#include "main.h"

Nrf24l01_PRXStr 	prx 		= {0};				// NRF���սṹ��
u8 					txbuf[] 	= {1};				// nrf���ͻ���
u8 					rxbuf[10] 	= {0};				// nrf���ջ���
u8 					flag_wake 	= 1;				// ���ѱ�־
u32 				dm_counter 	= 0;				// �������DM��������
TimerLinkStr 		timer2 		= {0};				// ����Ķ�ʱ��
TaskLinkStr			task_link 	= {0};
TaskLinkStr* 		tasklink 	= &task_link;		// �����б�
u16 				sleeptime	= TIM_SLEEP;		// ˯�ߵ���ʱ

u32 				threshold_30 	= 0;			// 30�����޶�����ֵ
BATStr 				bat = {0};						// ��ؽṹ��
TaskStr* 			taskBatControl 	= {0};	
TaskStr* 			taskYS			= {0};			// YS��������
TaskStr* 			taskKeyScan		= {0};			// KEY ɨ��

TaskStr* 			taskInMain   	={0};


JugeCStr 			beep = {0};
JugeCStr 			LEDAM_juge = {0};
JugeCStr 			LEDY30_juge = {0};
JugeCStr 			NRFpowon = {0};
JugeCStr 			NRFpowoff = {0};

u16					amtime = 0;
u16					y30time = 0;

extern u32 			counter_BH	;				// BH����
extern u8 			flag_no30;
extern keyStr 		key_AM;
extern keyStr 		key_Y30;
extern u8			flag_YS_isno;
extern keyStr 		key_DM;


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
	NRF24L01_PWR(0); 
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// �ر�SPIʱ��
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


void BeepStop()
{
	#if BEEP_SW > 0
	GPIO_RESET(GPIO_BEEP);
	#else
	GPIO_SET(GPIO_BEEP);
	#endif
}

void FunInMain()
{	
	
	CheckBC1BC2();		// �����λ
	YS_Control();		// YS���������
	MotorControl();		// ����˶�
	BatControl(&bat,tasklink,taskBatControl);	// ��Դ����
	
}
//����������
void KeyFun();
void main()
{

  	CLK_GPIO_Init();								// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	delay_ms(1000);									// �ȴ�ϵͳ�ȶ�
	UART_INIT(115200);

	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	prx.txbuf[0] =39; //�յ��ظ���Ϣ����䡮9��
	LEN_RED_Close();
	LEN_GREEN_Close();
	FlashData_Init();
//	tasklink = SingleCycList_Create();				// ����һ������ѭ������
	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskMotor = OS_CreatTask(&timer2);				// ���������������
	taskYS = OS_CreatTask(&timer2);					// ����YS�������� ��ÿ2����һ��
	taskKeyScan = OS_CreatTask(&timer2);			// ��������ɨ������
	taskInMain = OS_CreatTask(&timer2);				// ������������������
	OS_AddFunction(taskInMain,FunInMain,50);	
	OS_AddTask(tasklink,taskInMain);
	Make_SysSleep();								// ϵͳ��������״̬
	FL_GPIO_Init();
	enableInterrupts();                             // ʹ���ж�
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
		FLASH_ProgramWord(ADDR_DM,dm_counter);		// д��FLASH
		debug("dm_counter =0x%x%x\r\n",(u16)(dm_counter>>16),(u16)(dm_counter));
	}
	
	CheckWindowState();								// ��һ�´���λ��
	Key_GPIO_Init();								// ����������ʼ��
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
				if(flag_exti)	Key_ScanLeave();            // ���ֳ���
				OS_Task_Run(tasklink);
				
				/*nrf���պ���*/
				if(prx.hasrxlen != 0)
				{
				  debug("hasrxlen = %d :\r\n",prx.hasrxlen);		
					for(u8 i=0;i<prx.hasrxlen;i++)
					  {
						debug("rxbuf[%d]=%d	",i,prx.rxbuf[i]);
					  }
					
					debug("\r\n##################################\r\n");
					
					prx.hasrxlen = 0;
				}	
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
	
	u32 systime = OS_TimerFunc(&timer2);						// ��ʱ���ں���
	
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
	Juge_counter(&jugeYS,TIM__YS_D);
	//30
	if(flag_no30 && (systime > threshold_30)) flag_no30 = 0;	

	//��YS��ʱ����
	Juge_counter(&jugeYS_No,TIM_OPEN);
	//YS 30���Ӳ���Ӧ
	Juge_counter(&YS_30, (u32)60000*ys_timer30);
	//BEEP
	if(Juge_counter(&beep,130)) BeepStop();
	//LEDAM
	if(Juge_counter(&LEDAM_juge,amtime)) LEN_GREEN_Close();	
	//LEDY30
	if(Juge_counter(&LEDY30_juge,y30time)) LEN_RED_Close();	
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
	
//  	TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}


