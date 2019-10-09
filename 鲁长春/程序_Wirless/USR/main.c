/**
	2019-8-16
	Ò£¿ØÆ÷³ÌÐò

*/
#include "uhead.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_flash.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "keyboard.h"
#include "pwm.h"
#include "stmflash.h"

#define		PRESS_Y30	0X01
#define		PRESS_MOTZ	0X02
#define		PRESS_MOTY	0X04

#define	TIM_MAXDELAY	1100

Nrf24l01_PTXStr ptx = {0};
u8 rxbuf[5] = {0};

u8 		keyval = 0;
u8 		flag_pwm = 0;		// ´¥ÃþPWM¿ªÆô±êÖ¾
u8 		pwm = 1;
u8 		pwm_dir = 0;		// 0 ×Ô¼õ£¬1×ÔÔö
u8 		flag_wake = 1;
u32 		systime = 0;
u8 		pressKey = 0;
u32 		presstime = 0;
u8		is_DM = 0;		// ±£´æÊÇ·ñÅä¶ÔÐÅÏ¢

extern 		u32 		DM_time;


void RCC_Config()
{
   //æ—¶é’Ÿåˆå§‹åŒ– 16mhz                                          
    CLK->CKDIVR  &= 0XE7;   
    CLK->CKDIVR &= 0xF8;    
    CLK->ICKCR |= CLK_ICKCR_HSION;     
// //   CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1,DISABLE);    
// //   CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_USART1,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_BEEP,DISABLE);    
//    CLK_PeripheralClockConfig(CLK_Peripheral_DAC,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1,DISABLE);    
//    CLK_PeripheralClockConfig(CLK_Peripheral_RTC,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_LCD,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_DMA1,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_COMP,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_BOOTROM,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_AES,DISABLE);    
//    CLK_PeripheralClockConfig(CLK_Peripheral_TIM5,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_SPI2,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_USART2,DISABLE);    
//    CLK_PeripheralClockConfig(CLK_Peripheral_USART3,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_BOOTROM,DISABLE);
//    CLK_PeripheralClockConfig(CLK_Peripheral_CSSLSE,DISABLE); 
}

void FreeGPIO_Config()
{
//  GPIO_Init(GPIOA,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Slow);
//  GPIO_Init(GPIOA,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
//  GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Slow);
//  GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Slow);
//  GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
//  GPIO_Init(GPIOA,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Slow);
//  
}

// Ö¸Ê¾µÆ³õÊ¼»¯
void Init_LedGPIO(void)
{
	GPIO_Init(Z_LED,GPIO_Mode_Out_PP_High_Slow);
//	GPIO_Init(T_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_RESET(Z_LED);
//	GPIO_RESET(T_LED);
	delay_ms(1000);
	GPIO_SET(Z_LED);
//	GPIO_SET(T_LED);
	  
}

// ´¥ÃþIO³õÊ¼»¯£¬ÉÏÉýÑØ´¥·¢
void Init_TOUCHGPIO(void)
{
	GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
	disableInterrupts();
    	EXTI_SelectPort(EXTI_Port_B);
	EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Rising);   
	GPIO_RESET(TOUCH_IO);
    	enableInterrupts();                                           //Ê¹ÄÜÖÐ¶Ï
}

//TIM3³õÊ¼»¯
void TIM3_INIT()
{

    TIM3_ARRPreloadConfig(ENABLE);
    TIM3_ITConfig(TIM3_IT_Update, ENABLE);
    TIM3_ClearITPendingBit(TIM3_IT_Update); 
    TIM3_Cmd(ENABLE);

}

//ÈÃÏµÍ³ÐÝÃß
void Make_SysSleep()
{
    	debug(" Sleep \r\n");
	TIM3_Cmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
	systime = 0;
	NRF24L01_PWR(0); 
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// ¹Ø±ÕSPIÊ±ÖÓ
	CLK_LSICmd(ENABLE);						// Ê¹ÄÜLSI
	//CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_64);  	// RTCÊ±ÖÓÔ´LSI
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// µÈ´ýLSI¾ÍÐ÷
	PWR_UltraLowPowerCmd(ENABLE); 					// Ê¹ÄÜµçÔ´µÄµÍ¹¦ºÄÄ£Ê½
	PWR_FastWakeUpCmd(ENABLE);
	flag_wake = 0;
}
//ÈÃÏµÍ³»½ÐÑ
void MakeSysWakeUp()
{
    	NRF24L01_PWR(1);
	flag_wake = 1;
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,ENABLE);
	TIM3_Cmd(ENABLE);
//	systime = 0;
	debug(" WakeUp \r\n");
}

//Í¨¹ýNRFÏòÖ÷°å·¢ËÍÃüÁîº¯Êý
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8 cmd , u8 mes)
{
    u8 txbuf[5] = {0};
    txbuf[0] = cmd;
    txbuf[4] = mes;
    NRF_AutoAck_TxPacket(ptx,txbuf,5);
}

void main()
{    

	RCC_Config();
	FreeGPIO_Config();
	Key_GPIO_Init();

	delay_ms(500);
	UART_INIT(115200);					
	InitNRF_AutoAck_PTX(&ptx,rxbuf,sizeof(rxbuf),BIT_PIP0,RF_CH_HZ);

	Init_LedGPIO();	
	Init_TOUCHGPIO();
	PWM_Init();			//ºôÎüµÆ£¬PWM³õÊ¼»¯
	TIM3_INIT();			//¶¨Ê±Æ÷3,1ms
	
	NRF_CreatNewAddr(ADDRESS2);
	debug("New ID:%d,%d,%d,%d,%d",ADDRESS2[0],ADDRESS2[1],ADDRESS2[2],ADDRESS2[3],ADDRESS2[4]);
    while(1)
    {    
	if(flag_wake)
	{
	  //°´¼ü¼ì²â
	   if(flag_exti) Key_ScanLeave();
	   if(keyval != KEY_VAL_NULL && keyval != KEY_VAL_DUIMA && keyval != KEY_VAL_AM && keyval != KEY_VAL_POW_CA)
	   {
		 switch(keyval)
		 {
		     case KEY_VAL_Y30: pressKey = PRESS_Y30;presstime = systime + TIM_MAXDELAY;
		   break;
		     case KEY_VAL_I30:
		     switch(pressKey){
			 case PRESS_Y30:debug("NRF Y30 ÑÓÊ±30·ÖÖÓ\r\n");NRF_SendCMD(&ptx,CMD_Y30, MES_Y30_3_1);
			 break;
			 case PRESS_MOTZ:debug("NRF Âí´ï¹Ø1/3\r\n");NRF_SendCMD(&ptx,CMD_Z, MES_Z_3_1);
			 break;
			 case PRESS_MOTY:debug("NRF Âí´ï¿ª1/3\r\n");NRF_SendCMD(&ptx,CMD_Y, MES_Y_3_1);
			 break;
		     };
		     pressKey = 0;
		   break;
			case KEY_VAL_I60:
			switch(pressKey){
			 case PRESS_Y30:debug("NRF Y30 ÑÓÊ±60·ÖÖÓ\r\n");NRF_SendCMD(&ptx,CMD_Y30, MES_Y30_3_2);
			 break;
			 case PRESS_MOTZ:debug("NRF Âí´ï¹Ø2/3\r\n");NRF_SendCMD(&ptx,CMD_Z, MES_Z_3_2);
			 break;
			 case PRESS_MOTY:debug("NRF Âí´ï¿ª2/3\r\n");NRF_SendCMD(&ptx,CMD_Y, MES_Y_3_2);
			 break;
		     };
		     pressKey = 0;
		   break;
			case KEY_VAL_I100:
			switch(pressKey){
			 case PRESS_Y30:debug("NRF Y30 ÑÓÊ±90·ÖÖÓ\r\n");NRF_SendCMD(&ptx,CMD_Y30, MES_Y30_3_3);
			 break;
			 case PRESS_MOTZ:debug("NRF Âí´ïÈ«¹Ø\r\n");NRF_SendCMD(&ptx,CMD_Z, MES_Z_3_3);
			 break;
			 case PRESS_MOTY:debug("NRF Âí´ïÈ«¿ª\r\n");NRF_SendCMD(&ptx,CMD_Y, MES_Y_3_3);
			 break;
		     };
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
		if(flag_exti == 0 && flag_pwm == 0 &&  keyval == KEY_VAL_NULL && pressKey == 0)Make_SysSleep();		
	}else
	{
		halt();
		MakeSysWakeUp();
	}
		


    }   
    
}


#ifdef USE_FULL_ASSERT
void assert_failed(u8* file,u32 line)
{
  debug("assert_failed : %s , line:%d\r\n",file,line);
  while(1);
}
#endif
 

//TIM3¸üÐÂÖÐ¶Ï,1ms
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler,21)
{      
    systime ++;
    if(pressKey)		if(systime > presstime) pressKey = 0;

  //ºôÎüµÆPWM
  if(flag_pwm )
  {
	static u8 zled_counter = 0;
	zled_counter ++;
	if(zled_counter>15)
	{
		zled_counter = 0;
		if(pwm_dir)
		{
			pwm ++;
			if(pwm>100)
			{
				pwm = 100;
				pwm_dir = 0;
			}				
		}
		else
		{
			pwm --;
			if(pwm == 1) flag_pwm = 0;
		} 
		PWM_SetDutyCycle(pwm);
	}
	 //Ã»ÓÐ´¥Ãþ£¬¹Ø±ÕºôÎüµÆ
	  if((GPIO_READ(TOUCH_IO) == RESET) && pwm == 100) 
	  {
	  	PWM_Status(PWM_OFF);
		zled_counter = 0;
		pwm = 1;
		flag_pwm = 0;
	  }
  }
  
  TIM3_ClearITPendingBit(TIM3_IT_Update);  
}
//´¥ÃþIO
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
  	if(GPIO_READ(TOUCH_IO) != RESET)	
	{
	
		//systime = 0;
		PWM_Status(PWM_ON);
		pwm = 1;
		PWM_SetDutyCycle(pwm);
		flag_pwm = 1;
		pwm_dir = 1;
		debug("in touch TI \r\n");
	}
	
	EXTI_ClearITPendingBit (EXTI_IT_Pin1);  
       
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