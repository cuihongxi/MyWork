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

Nrf24l01_PTXStr ptx = {0};

#define SIZE_TX 3
u8 rxbuf[32] = {0};
u8 txbuf[SIZE_TX] = {0};


u8 keyval = 0;
u8 flag_pwm = 0;		//´¥ÃþPWM¿ªÆô±êÖ¾
u8 zled_counter = 0;
u8 pwm = 1;
u8 pwm_dir = 0;			//0 ×Ô¼õ£¬1×ÔÔö
u8 flag_wake = 1;
//æ—¶é’Ÿé…ç½®
void RCC_Config()
{
   //æ—¶é’Ÿåˆå§‹åŒ– 16mhz                                          
    CLK->CKDIVR  &= 0XE7;   
    CLK->CKDIVR &= 0xF8;    
    CLK->ICKCR |= CLK_ICKCR_HSION;     
 //   CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1,DISABLE);    
 //   CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_BEEP,DISABLE);    
    CLK_PeripheralClockConfig(CLK_Peripheral_DAC,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1,DISABLE);    
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_LCD,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_DMA1,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_COMP,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_BOOTROM,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_AES,DISABLE);    
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM5,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI2,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART2,DISABLE);    
    CLK_PeripheralClockConfig(CLK_Peripheral_USART3,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_BOOTROM,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_CSSLSE,DISABLE); 
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
//	debug("sleep:\r\n");
//	LowPowerSet();

	NRF24L01_PWR(0); 
	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);		// ¹Ø±ÕSPIÊ±ÖÓ
	CLK_LSICmd(ENABLE);											//Ê¹ÄÜLSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTCÊ±ÖÓÔ´LSI£¬
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //µÈ´ýLSI¾ÍÐ÷
	PWR_UltraLowPowerCmd(ENABLE); 								//Ê¹ÄÜµçÔ´µÄµÍ¹¦ºÄÄ£Ê½
	PWR_FastWakeUpCmd(ENABLE);

	flag_wake = 0;
}
//ÈÃÏµÍ³»½ÐÑ
void MakeSysWakeUp()
{
	flag_wake = 1;
	debug(" WakeUp \r\n");
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
	TIM3_INIT();		//¶¨Ê±Æ÷3,1ms

    while(1)
    {    
		if(flag_wake)
		{
			//°´¼ü¼ì²â
		  if(flag_exti)
		  {      
				Key_ScanLeave();
		  }
		   if(keyval != KEY_VAL_NULL)
		   {
			 debug("\r\n");
			 switch(keyval)
			 {
				case KEY_VAL_AM:	debug("KEY_VAL_AM");NRF_AutoAck_TxPacket(&ptx,"_AM",3);
			   break;
				case  KEY_VAL_POW_CA:debug("KEY_VAL_POW_CA");NRF_AutoAck_TxPacket(&ptx,"POW",3);
			   break;
				case KEY_VAL_Y30:debug("KEY_VAL_Y30");NRF_AutoAck_TxPacket(&ptx,"Y30",3);
			   break;
				case KEY_VAL_I30:debug("KEY_VAL_I30");NRF_AutoAck_TxPacket(&ptx,"I30",3);
			   break;
				case KEY_VAL_I60:debug("KEY_VAL_I60");NRF_AutoAck_TxPacket(&ptx,"I60",3);
			   break;
				case KEY_VAL_I100:debug("KEY_VAL_I100");NRF_AutoAck_TxPacket(&ptx,"100",3);
			   break;
				case KEY_VAL_MOTZ:debug("KEY_VAL_MOTZ");NRF_AutoAck_TxPacket(&ptx,"MOZ",3);
			   break;
				case KEY_VAL_MOTY:debug("KEY_VAL_MOTY");NRF_AutoAck_TxPacket(&ptx,"MOY",3);
			   break;
				case KEY_VAL_DUIMA:debug("KEY_VAL_DUIMA");NRF_AutoAck_TxPacket(&ptx,"DM_",3);
			   break;
			   
			 }
			 keyval = KEY_VAL_NULL;
			debug("\r\n");
		   }	  
			if(flag_exti == 0 && flag_pwm == 0)Make_SysSleep();		
		}else
		{
			halt();
			MakeSysWakeUp();
		}
		


    }   
    
}


//è‡ªåŠ¨å”¤é†’
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{       
   RTC_ClearITPendingBit(RTC_IT_WUT);  
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
 	//ºôÎüµÆPWM
  if(flag_pwm )
  {
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
		TIM3_Cmd(DISABLE);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
	  }
  }
  
  TIM3_ClearITPendingBit(TIM3_IT_Update);  
}
//´¥ÃþIO
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
  	if(GPIO_READ(TOUCH_IO) != RESET)	
	{
		TIM3_Cmd(ENABLE);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,ENABLE);
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