#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "stm8l15x_clk.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "lowpower.h"
#include "stm8l15x_rtc.h"
#include "stm8l15x_tim2.h"
#include "CUI_RTOS.H"

Nrf24l01_PRXStr prx = {0};		//NRF���սṹ��
u8 txbuf[] = {1};				//nrf���ͻ���
u8 rxbuf[100] = {0};			//nrf���ջ���

uint16_t BATdat;				//��ص���
u8 flag_wake = 1;				//���ѱ�־
u8 flag = 0;
u16 time2_counter = 0;

TimerLinkStr timer2 = {0};

//��ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
	CLK_Change2HSI();		//�л�HSIʱ��

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

//��������ص���
void    OpenCheckBat()
{
  //  BATdat = Get_ADC_Dat(Battery_Channel);
    LSI_delayms(2000);     
    BATdat = Get_ADC_Dat(Battery_Channel);
    if(BATdat < VALVE_BAT_ADCval)                 //7.2V�������
    {
        //��Ƴ�����˸��3��1��
        u8 i = 3;
        while(i--)
        {
         
        }
    }else
    {
        //�̵���3��
        u8 i = 3;
        while(i--)
        {
         
        }       
    }
}
void LED_RED_ON(void)
{
	LED_RED_Open(1);
	debug("1 : LED_RED_Open\r\n");	
}

void LED_RED_OFF(void)
{
	LED_RED_Open(0);
debug("1 : LED_RED_Close\r\n");
}

void LED_GREEN_ON(void)
{
	LED_GREEN_Open(1);
	debug("2 : LED_GREEN_Open\r\n");
}
void LED_GREEN_OFF(void)
{
	LED_GREEN_Open(0);
	debug("2 : LED_GREEN_Close\r\n");
}
void main()
{
  	CLK_GPIO_Init();										// �͹���ʱ�Ӻ�GPIO��ʼ��,2MHZ
	UART_INIT(115200);
//	debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����	
	GPIO_ADC_Init();
	LED_RED_Open(flag);	
	TIM2_INIT();
	
	TaskLinkStr* tasklink = SingleCycList_Create();
	TaskStr* task1 = OS_CreatTask(&timer2);
	OS_AddFunction(task1,LED_RED_ON,500);
	OS_AddFunction(task1,LED_RED_OFF,500);		

	TaskStr* task2 = OS_CreatTask(&timer2);
	OS_AddFunction(task2,LED_GREEN_OFF,2000);	
	OS_AddFunction(task2,LED_GREEN_ON,2000);
	
	OS_AddTask(tasklink,task1);	
	OS_AddTask(tasklink,task2);
	while(1)
	{
		OS_Task_Run(tasklink);
	}
	
	
	
	
	while(1)
	{
      //���ߺ���
      if(flag_wake == 0)
      {
		  debug("sleep:\r\n");
            TIM2_Cmd(DISABLE);
            CLK_LSICmd(ENABLE);											//ʹ��LSI
            CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_16);  //RTCʱ��ԴLSI��16��Ƶ=2375
            while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //�ȴ�LSI����
            RTC_WakeUpCmd(DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTCʱ���ſ�ʹ��
            RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);   
			
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //�����ж�
            RTC_SetWakeUpCounter(149*3);                     			//���Ѽ��
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //���Ѷ�ʱ���ж�ʹ��
            RTC_WakeUpCmd(ENABLE);                                      //RTC����ʹ��            
 //           DATA_Init();               
            halt();
            debug("\r\nwake up\r\n");
//            if(flag_exti == 0)Wake_CheckHT();             //��ʱ���Ѻ���
//            if(flag_exti)               //��������
//            {
//                flag_wake = 1;
//                //ʱ�ӳ�ʼ��                                           
//                CLK->CKDIVR  &= 0XE7;   
//                CLK->CKDIVR &= 0xF8;    
//                CLK->ICKCR |= CLK_ICKCR_HSION;      // CLK_HSICmd(ENABLE);
//                NRF24L01_GPIO_Init();
//                NRF24L01_PWR(1);
//                FreeNRFGPIO();                
//                TIM2_Cmd(ENABLE);
//            } 
        
      }
	  else   
	  {
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
	  }	    

	}
}

//IRQ �жϷ�����
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	prx.IRQCallBack(&prx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}
//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{

	debug("\r\n##################################\r\n");
   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}
//TIM2�����ж�,1ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	OS_TimerFunc(&timer2);							// ��ʱ���ں���
  	TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}
