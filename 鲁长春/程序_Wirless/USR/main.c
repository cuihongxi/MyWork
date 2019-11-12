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

#define	        TIM_MAXDELAY	2000

#define         IRQ_PERIOD      500   

Nrf24l01_PRXStr 	prx 		= {0};				// NRF接收结构体
Nrf24l01_PTXStr 	ptx 		= {0};				// NRF发送结构体

u8      rxbuf[7] = {0};
u8      txbuf[7] = {0};
u8      nrfaddr[5];
u8 	keyval = 0;
u8 	flag_wake = 1;
u32 	systime = 0;
u8 	pressKey = 0;
u32 	presstime = 0;
u8	is_DM = 0;		// 保存是否配对信息
u8	flag_duima = 0;
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

	NRF24L01_PWR(0); 						// 关闭NRF的电源
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// 关闭SPI时钟
	CLK_LSICmd(ENABLE);						// 使能LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTC时钟源LSI，
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// 等待LSI就绪
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTC时钟门控使能
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19K时钟频率
	
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 开启中断
	RTC_SetWakeUpCounter(9500);                     		// 唤醒间隔	500mS
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
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}
//DM模式自动接收完成回调函数
void dmRXD_CallBack(Nrf24l01_PRXStr* prx) 
{
      prx->rxlen = NRF24L01_GetRXLen();	
      NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf,prx->rxlen);	//读取数据
      NRF24L01_RX_AtuoACKPip(prx->rxbuf,prx->rxlen,prx->pip);	//填充应答信号		
      NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// 清除RX_DS中断标志
      debug("RX_OK ");
      flag_duima = 0;
      
}
void DM_Mode()
{
       flag_duima = 1;
      address = ADDRESS1;
      InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
      NRF24L01_PWR(1);
      prx.RXDCallBack = dmRXD_CallBack;
      while(flag_duima)
      {
        
      }
 	InitNRF_AutoAck_PTX(&ptx,rxbuf,sizeof(rxbuf),BIT_PIP0,RF_CH_HZ);
        NRF24L01_GPIO_Lowpower();     
}
void main()
{    

	RCC_Config();
	Key_GPIO_Init();
	//delay_ms(500);
	UART_INIT(115200);	
      //  FlashData_Init();
        address = nrfaddr;
	InitNRF_AutoAck_PTX(&ptx,rxbuf,sizeof(rxbuf),BIT_PIP0,RF_CH_HZ);
	Init_LedGPIO();	
	//NRF_CreatNewAddr(ADDRESS2);
	//debug("addr:%d,%d,%d,%d,%d",address[0],address[1],address[2],address[3],address[4]);
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
		     case KEY_VAL_Y30: pressKey = PRESS_Y30;presstime = systime + TIM_MAXDELAY;//debug("systime = %d,presstime = %d\n",systime,presstime);
		   break;
		     case KEY_VAL_I30:
                       //debug("systime = %d,presstime = %d\n",systime,presstime);
                       if(systime<=presstime)
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
                          //debug("systime = %d,presstime = %d\n",systime,presstime);
                        if(systime<=presstime)
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
                        //  debug("systime = %d,presstime = %d\n",systime,presstime);
                       if(systime <= presstime)
                       {
                         
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
			case KEY_VAL_MOTZ:pressKey = PRESS_MOTZ;presstime = systime + TIM_MAXDELAY;//debug("systime = %d,presstime = %d\n",systime,presstime);
		   break;
			case KEY_VAL_MOTY:pressKey = PRESS_MOTY;presstime = systime + TIM_MAXDELAY;//debug("systime = %d,presstime = %d\n",systime,presstime);
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
      if(flag_duima == 0)	
        ptx.IRQCallBack(&ptx);
      else
        prx.IRQCallBack(&prx);
  }
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}             