/**
	2019-8-16
	遥控器程序

*/
#include "uhead.h"
#include "MYCLK.h"
#include "stm8l15x_flash.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "keyboard.h"
#include "pwm.h"
#include "stmflash.h"
#include "UDATA.H"
#include "stm8l15x_rtc.h"

#define		PRESS_Y30	0X01
#define		PRESS_MOTZ	0X02
#define		PRESS_MOTY	0X04

#define	        TIM_MAXDELAY	1000
#define         IRQ_PERIOD      1000   

Nrf24l01_PTXStr         ptx = {0};

u8      rxbuf[7] = {0};
u8      nrfaddr[5];
u8 	keyval = 0;
u8 	flag_wake = 1;
int 	systime = 0;
u8 	pressKey = 0;
int 	presstime = 0;
u8	is_DM = 0;		// 保存是否配对信息

extern 		u32 		DM_time;


void RCC_Config()
{
  CLK_Change2HSI();				//切换HSI时钟 
}


// 指示灯初始化
void Init_LedGPIO(void)
{
	GPIO_Init(Z_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_Init(T_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_RESET(Z_LED);
	GPIO_RESET(T_LED);
	delay_ms(1000);
	GPIO_SET(Z_LED);
	GPIO_SET(T_LED);
	  
}

//让系统休眠
void Make_SysSleep()
{
    	//debug(" Sleep \r\n");
	systime = 0;
	NRF24L01_PWR(0); 						// 关闭NRF的电源
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// 关闭SPI时钟
	CLK_LSICmd(ENABLE);						// 使能LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTC时钟源LSI，
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// 等待LSI就绪
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTC时钟门控使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19K时钟频率
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 开启中断
	RTC_SetWakeUpCounter(19000);                     			// 唤醒间隔	1000mS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 唤醒定时器中断使能
	RTC_WakeUpCmd(ENABLE);                                      	// RTC唤醒使能  
	PWR_UltraLowPowerCmd(ENABLE); 					// 使能电源的低功耗模式
	PWR_FastWakeUpCmd(ENABLE);
}

//通过NRF向主板发送命令函数
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
{
    u8 txbuf[7] = {0};
    NRF24L01_PWR(1);
    txbuf[0] = addr[0];
    txbuf[1] = addr[1];
    txbuf[2] = addr[2];
    txbuf[3] = addr[3];
    txbuf[4] = addr[4];
    txbuf[5] = cmd;
    txbuf[6] = mes;
    NRF_AutoAck_TxPacket(ptx,txbuf,7);
  
}

void main()
{    

	RCC_Config();
	Key_GPIO_Init();

	delay_ms(500);
	UART_INIT(115200);	
        FlashData_Init();
        address = nrfaddr;
	InitNRF_AutoAck_PTX(&ptx,rxbuf,sizeof(rxbuf),BIT_PIP0,RF_CH_HZ);
	Init_LedGPIO();	
	NRF_CreatNewAddr(ADDRESS2);
	debug("addr:%d,%d,%d,%d,%d",address[0],address[1],address[2],address[3],address[4]);
        NRF24L01_GPIO_Lowpower();
       Make_SysSleep();

    while(1)
    {    
          halt();
	  //按键检测
	   if(flag_exti) Key_ScanLeave();
	   if(keyval != KEY_VAL_NULL && keyval != KEY_VAL_DUIMA && keyval != KEY_VAL_AM && keyval != KEY_VAL_POW_CA)
	   {
		 switch(keyval)
		 {
		     case KEY_VAL_Y30: pressKey = PRESS_Y30;presstime = systime + TIM_MAXDELAY;
		   break;
		     case KEY_VAL_I30:
                       if(systime<presstime)
                       {
                         switch(pressKey){
                             case PRESS_Y30:debug("延时30分钟");NRF_SendCMD(&ptx,address,CMD_Y30, MES_Y30_3_1);
                             break;
                             case PRESS_MOTZ:debug("马达关1/3");NRF_SendCMD(&ptx,address,CMD_Z, MES_Z_3_1);
                             break;
                             case PRESS_MOTY:debug("马达开1/3");NRF_SendCMD(&ptx,address,CMD_Y, MES_Y_3_1);
                             break;
                         };                      
                       }
                     presstime = 0;
		     pressKey = 0;
		   break;
			case KEY_VAL_I60:
                        if(systime<presstime)
                       {                         
			switch(pressKey){
			 case PRESS_Y30:debug("延时60分钟");NRF_SendCMD(&ptx,address,CMD_Y30, MES_Y30_3_2);
			 break;
			 case PRESS_MOTZ:debug("马达关2/3");NRF_SendCMD(&ptx,address,CMD_Z, MES_Z_3_2);
			 break;
			 case PRESS_MOTY:debug("马达开2/3");NRF_SendCMD(&ptx,address,CMD_Y, MES_Y_3_2);
			 break;
                        };
                       }
                     presstime = 0;
		     pressKey = 0;
		   break;
			case KEY_VAL_I100:
                       if(systime < presstime)
                       {
                         debug("systime = %d,presstime = %d\n",systime,presstime);
			switch(pressKey){
                                 case PRESS_Y30:debug("延时90分钟");NRF_SendCMD(&ptx,address,CMD_Y30, MES_Y30_3_3);
                                 break;
                                 case PRESS_MOTZ:debug("马达全关");NRF_SendCMD(&ptx,address,CMD_Z, MES_Z_3_3);
                                 break;
                                 case PRESS_MOTY:debug("马达全开");NRF_SendCMD(&ptx,address,CMD_Y, MES_Y_3_3);
                                 break;
                            };
                       }
                     presstime = 0;
		     pressKey = 0;
		   break;
			case KEY_VAL_MOTZ:pressKey = PRESS_MOTZ;presstime = systime + TIM_MAXDELAY;
		   break;
			case KEY_VAL_MOTY:pressKey = PRESS_MOTY;presstime = systime + TIM_MAXDELAY;
		   break;
		   
		 }
		 keyval = KEY_VAL_NULL;
		debug("\r\n");
	   }	  		
    }   
    
}


#ifdef USE_FULL_ASSERT
void assert_failed(u8* file,u32 line)
{
  debug("assert_failed : %s,line:%d\r\n",file,line);
  while(1);
}
#endif
 

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
        systime += IRQ_PERIOD;
   	RTC_ClearITPendingBit(RTC_IT_WUT);  

}

//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
  if(GPIO_READ(NRF24L01_IRQ_PIN)== RESET)
  {
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,ENABLE);	
  	ptx.IRQCallBack(&ptx);
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
	
  }
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}             