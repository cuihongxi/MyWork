/**
	2019-8-16
	遥控器程序

*/
#include "uhead.h"
#include "MYCLK.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "keyboard.h"
#include "UDATA.H"
#include "stm8l15x_rtc.h"
#include "stm8l15x_itc.h"

#define		PRESS_Y30	0X01
#define		PRESS_MOTZ	0X02
#define		PRESS_MOTY	0X04

#define	    TIM_MAXDELAY	2000
#define		PERIOD_DM	3000		//ms, 对码周期

Nrf24l01_PTXStr 	ptx 		= {0};				// NRF发送结构体

u8      TXrxbuf[7] = {0};
u8      TXtxbuf[7] = {0};

u8 		keyval = 0;
u8 		flag_wake = 1;
u32 	systime = 0;
u8 		pressKey = 0;
u32 	presstime = 0;
u8		flag_duima = 0;
u8      DM_num = 0;
u8		LEDtimes = 0;
u32		sendtime = 0;
u8 		flag = 0;
u8 		db 	= 	1;


void LEN_GREEN_Open()
{
	GPIO_RESET(_T_LED);
}

void LEN_GREEN_Close()
{
	GPIO_SET(_T_LED);
}

void LEN_RED_Open()
{
	GPIO_RESET(Z_LED);
}

void LEN_RED_Close()
{
	GPIO_SET(Z_LED);
}
// 指示灯初始化
void Init_LedGPIO(void)
{
	GPIO_Init(Z_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_Init(_T_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_RESET(Z_LED);
	GPIO_RESET(_T_LED);
	delay_ms(1000);
	GPIO_SET(Z_LED);
	GPIO_SET(_T_LED);
	  
}

//让系统休眠
void Make_SysSleep()
{
	NRF24L01_PWR(0); 						// 关闭NRF的电源
	CLK_LSICmd(ENABLE);						// 使能LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTC时钟源LSI，
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// 等待LSI就绪
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTC时钟门控使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19K时钟频率
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           // 开启中断
	RTC_WakeUpCmd(DISABLE);
	RTC_SetWakeUpCounter(2375);                     		// 唤醒间隔	125mS
//	RTC_SetWakeUpCounter(9500);                     		// 唤醒间隔	500mS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 唤醒定时器中断使能
	RTC_WakeUpCmd(ENABLE);                                      	// RTC唤醒使能  
	PWR_UltraLowPowerCmd(ENABLE); 					// 使能电源的低功耗模式
	PWR_FastWakeUpCmd(ENABLE);
}

//通过NRF向主板发送命令函数
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
{
    NRF24L01_PWR(1);
    ptx->txbuf[0] = addr[0];
    ptx->txbuf[1] = addr[1];
    ptx->txbuf[2] = addr[2];
    ptx->txbuf[3] = addr[3];
    ptx->txbuf[4] = addr[4];
    ptx->txbuf[5] = cmd;
    ptx->txbuf[6] = mes;
	
	sendtime = systime;
	GPIO_RESET(_T_LED);
	NRF24L01_ClearTXFIFO();
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}


// 触摸IO初始化，上升沿触发
void Init_TOUCHGPIO(void)
{
	GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
	disableInterrupts();
    EXTI_SelectPort(EXTI_Port_B);
	EXTI_SetPinSensitivity(EXTI_Pin_5,EXTI_Trigger_Falling);   
	GPIO_RESET(TOUCH_IO);
    enableInterrupts();  
}

void main()
{    
	Key_GPIO_Init();
	UART_INIT(115200);	
    Init_LedGPIO();    
	NRF_CreatNewAddr(ADDRESS2);
	
	u8 i = 0;
	for(i =0;i<5;i++)
	{
		debug("ADDRESS2[%d] = %x\r\n",i,ADDRESS2[i]);
	}
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
    ptx.txbuf = TXtxbuf;
	Init_TOUCHGPIO();
	Make_SysSleep();
	
//	IWDG_Enable();
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//	IWDG_SetPrescaler(IWDG_Prescaler_256);
//	IWDG_SetReload(0xFF);
//    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	//Init_TOUCHGPIO();

    while(1)
    {    
	  
       halt();	
	   if(LEDtimes)
	   {
		  LEDtimes --;
		  if(db == 1)
		  {
			  if(LEDtimes&0x01)
				LEN_GREEN_Open();
			  else LEN_GREEN_Close();		  
		  }else
		  {
			  if(LEDtimes&0x01)
				LEN_RED_Open();
			  else LEN_RED_Close();			  	
		  }
		  
	   }
	   if(flag_duima == 0)			// 非对码状态
	   {
		 	  //按键检测
	   		if(flag_exti) Key_ScanLeave();
			if(flag_touch)Key_TouchtLeave();
		   if(keyval != KEY_VAL_NULL && keyval != KEY_VAL_DUIMA && keyval != KEY_VAL_AM && keyval != KEY_VAL_POW_CA)
		   {
			 switch(keyval)
			 {
				 case KEY_VAL_Y30: pressKey = PRESS_Y30;presstime = systime + TIM_MAXDELAY;
			   break;
				 case KEY_VAL_I30:
						   if(systime<=presstime)
						   {
							 switch(pressKey){
								 case PRESS_Y30:	//debug("延时30分钟");
									NRF_SendCMD(&ptx,ADDRESS2,CMD_Y30, MES_Y30_3_1);
								 break;
								 case PRESS_MOTZ:	//debug("马达关1/3");
									NRF_SendCMD(&ptx,ADDRESS2,CMD_Z, MES_Z_3_1);
								 break;
								 case PRESS_MOTY:	//debug("马达开1/3");
									NRF_SendCMD(&ptx,ADDRESS2,CMD_Y, MES_Y_3_1);
								 break;
							 };                      
						   }
							presstime = 0;
							pressKey = 0;
			   break;
				case KEY_VAL_I60:
							if(systime<=presstime)
						   {                         
							switch(pressKey){
							 case PRESS_Y30://debug("延时60分钟");
							   NRF_SendCMD(&ptx,ADDRESS2,CMD_Y30, MES_Y30_3_2);
							 break;
							 case PRESS_MOTZ://debug("马达关2/3");
							   NRF_SendCMD(&ptx,ADDRESS2,CMD_Z, MES_Z_3_2);
							 break;
							 case PRESS_MOTY://debug("马达开2/3");
							   NRF_SendCMD(&ptx,ADDRESS2,CMD_Y, MES_Y_3_2);
							 break;
							};
						   }
							presstime = 0;
							pressKey = 0;
			   break;
				case KEY_VAL_I100:
						   if(systime <= presstime)
						   {
							 
									switch(pressKey){
									 case PRESS_Y30://debug("延时90分钟");
									   NRF_SendCMD(&ptx,ADDRESS2,CMD_Y30, MES_Y30_3_3);
									 break;
									 case PRESS_MOTZ://debug("马达全关");
									   NRF_SendCMD(&ptx,ADDRESS2,CMD_Z, MES_Z_3_3);
									 break;
									 case PRESS_MOTY://debug("马达全开");
									   NRF_SendCMD(&ptx,ADDRESS2,CMD_Y, MES_Y_3_3);
									 break;
									};
						   }
							presstime = 0;
							pressKey = 0;
			   break;
				case KEY_VAL_MOTZ:	pressKey = PRESS_MOTZ;
									presstime = systime + TIM_MAXDELAY;
									NRF_SendCMD(&ptx,ADDRESS2,CMD_Z, MES_Z);
			   break;
				case KEY_VAL_MOTY:	pressKey = PRESS_MOTY;
									presstime = systime + TIM_MAXDELAY;
									NRF_SendCMD(&ptx,ADDRESS2,CMD_Y, MES_Y);
			   break;
			   
			 }
			 if(keyval != KEY_VAL_Y30 && keyval != KEY_VAL_I30) keyval = KEY_VAL_NULL;
		   }  		   
	   }else
	   {
			debug("DM 模式\r\n");
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);			  	
			NRF_SendCMD(&ptx,ADDRESS2,CMD_DM,MES_DM);			
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
			flag_duima = 0;
			GPIO_SET(_T_LED);
	   }
	}   
    
}
 

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    systime += IRQ_PERIOD;
	//IWDG_ReloadCounter() ;
   	RTC_ClearITPendingBit(RTC_IT_WUT);  
}

//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
  if(GPIO_READ(NRF24L01_IRQ_PIN)== RESET)
  {	
//      if(flag_duima == 0)	
        ptx.IRQCallBack(&ptx);
//      else
//        prx.IRQCallBack(&prx);
  }
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}     