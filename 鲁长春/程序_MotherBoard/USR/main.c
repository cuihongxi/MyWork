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

Nrf24l01_PRXStr prx = {0};		//NRF接收结构体
u8 txbuf[] = {1};				//nrf发送缓存
u8 rxbuf[100] = {0};			//nrf接收缓存

uint16_t BATdat;				//电池电量
u8 flag_wake = 1;				//唤醒标志
u8 flag = 0;
u16 time2_counter = 0;

TimerLinkStr timer2 = {0};

//低时钟和GPIO初始化
void CLK_GPIO_Init()
{
	CLK_Change2HSI();		//切换HSI时钟

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

//开机检测电池电量
void    OpenCheckBat()
{
  //  BATdat = Get_ADC_Dat(Battery_Channel);
    LSI_delayms(2000);     
    BATdat = Get_ADC_Dat(Battery_Channel);
    if(BATdat < VALVE_BAT_ADCval)                 //7.2V开机检测
    {
        //红灯持续闪烁，3秒1次
        u8 i = 3;
        while(i--)
        {
         
        }
    }else
    {
        //绿灯闪3次
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
  	CLK_GPIO_Init();										// 低功耗时钟和GPIO初始化,2MHZ
	UART_INIT(115200);
//	debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_GPIO_Init();   										// 双色LED初始化
	MX830Motor_GPIOInit();                                 	// 马达IO配置	
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
      //休眠函数
      if(flag_wake == 0)
      {
		  debug("sleep:\r\n");
            TIM2_Cmd(DISABLE);
            CLK_LSICmd(ENABLE);											//使能LSI
            CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_16);  //RTC时钟源LSI，16分频=2375
            while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //等待LSI就绪
            RTC_WakeUpCmd(DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTC时钟门控使能
            RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);   
			
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //开启中断
            RTC_SetWakeUpCounter(149*3);                     			//唤醒间隔
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //唤醒定时器中断使能
            RTC_WakeUpCmd(ENABLE);                                      //RTC唤醒使能            
 //           DATA_Init();               
            halt();
            debug("\r\nwake up\r\n");
//            if(flag_exti == 0)Wake_CheckHT();             //定时唤醒函数
//            if(flag_exti)               //按键唤醒
//            {
//                flag_wake = 1;
//                //时钟初始化                                           
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
	  }	    

	}
}

//IRQ 中断服务函数
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	prx.IRQCallBack(&prx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}
//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{

	debug("\r\n##################################\r\n");
   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}
//TIM2更新中断,1ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
	OS_TimerFunc(&timer2);							// 定时器内函数
  	TIM2_ClearITPendingBit(TIM2_IT_Update); 
	
}
