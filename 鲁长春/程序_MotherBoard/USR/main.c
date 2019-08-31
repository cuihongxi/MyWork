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

Nrf24l01_PRXStr prx = {0};		// NRF���սṹ��
u8 txbuf[] = {1};				// nrf���ͻ���
u8 rxbuf[10] = {0};				// nrf���ջ���

u8 flag_wake = 1;				// ���ѱ�־
u8 flag_DM = 0;					// �������DM��������ʱ����־
u32 dm_counter = 0;				// �������DM��������

TimerLinkStr timer2 = {0};		// ����Ķ�ʱ��
TaskLinkStr* tasklink = {0};			// �����б�

u16 sleeptime = TIM_SLEEP;		// ˯�ߵ���ʱ

u8 flag_KEY_Z = 0;				//���ݸ���ﺯ������������val��������
u8 flag_KEY_Y = 0;

extern u32 	counter_BH	;		// BH����

extern u8 	flag_no30;
u32 threshold_30 = 0;			//30�����޶�����ֵ

BATStr bat = {0};						// ��ؽṹ��
TaskStr* taskBatControl;

//�������ݳ�ʼ��
void Wake_InitDat()
{
	flag_checkBat = 1;			//ÿ�λ��ѡ����BAT����һ��
	
}
//��ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
	CLK_Change2HSI();							//�л�HSIʱ��
	GPIO_Init(GPIO_DM,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(GPIO_38KHZ_BC2,GPIO_Mode_In_PU_No_IT);
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����	
	GPIO_ADC_Init();
	
}



//TIM2��ʼ��
void TIM2_INIT()
{
    enableInterrupts();                                                 //ʹ��ȫ���ж�
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);              //��ʱ��
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_Prescaler_1,TIM2_CounterMode_Up,2000);         //1ms 
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_ITConfig(TIM2_IT_Update, ENABLE);
    TIM2_ClearITPendingBit(TIM2_IT_Update); 
    TIM2_Cmd(ENABLE);

}

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
void main()
{
  	CLK_GPIO_Init();										// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	delay_ms(500);											//�ȴ�ϵͳ�ȶ�
	UART_INIT(115200);
//	debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
//	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_RED_Open(0);	
	Make_SysSleep();								// ϵͳ��������״̬
	Key_GPIO_Init();								// ����������ʼ��
	tasklink = SingleCycList_Create();
	
	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskMotor = OS_CreatTask(&timer2);				// ���������������	
	TaskStr* taskYS = OS_CreatTask(&timer2);		// ����YS�������� ��ÿ2����һ��


	//�ϵ���DM��ƽ�����ж���������г�ʱ��		
	if(GPIO_READ(GPIO_DM) == RESET)
	{
		//��ﷴת����λ
		motorStruct.command = FORWARD;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC1) != RESET);	//�ȴ�GPIO_38KHZ_BC1���ֵ͵�ƽ
		flag_DM = 1;
		motorStruct.command = BACK;
		MX830Motor_StateDir(&motorStruct);
		while(GPIO_READ(GPIO_38KHZ_BC2) != RESET);	//�ȴ�GPIO_38KHZ_BC2���ֵ͵�ƽ
		flag_DM = 0;
		debug("dm_counter = %lu\r\n",dm_counter);
	}
	FL_GPIO_Init();
	
	//���һ�ε�ص�ѹ
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	
	while(1)
	{
      if(flag_wake)
	  {
//			//�������ּ��
//			if(flag_exti)	Key_ScanLeave();                   //���ֳ���
//			//����������
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
//			//��Դ����
//			BatControl();
//			//����˶�
// 			MotorControl();
//			//�����λ
//			CheckBC1BC2();
//			//YS��D���������
//			YS_Control();	
//		//	debug("\r\nwake \r\n");	
//			OS_Task_Run(tasklink);	  
	  }
	  else	//���ߺ���
	  {
          
 //           DATA_Init();               
            halt();
			if(flag_wake == 0)
			{
				if(flag_exti)	Key_ScanLeave();                   //���ֳ���
				//����������
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
	if(flag_DM)	dm_counter ++;									// �����ʱ
	
	//��ص������
	if(systime >= bat.threshold) bat.flag = 1;
	// ������м�ʱ
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






