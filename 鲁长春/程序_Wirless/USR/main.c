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

Nrf24l01_PRXStr prx = {0};
u8 txbuf[] = {1};
u8 rxbuf[100] = {0};
u8 keyval = 0;
//æ—¶é’Ÿé…ç½®
void RCC_Config()
{
   //æ—¶é’Ÿåˆå§‹åŒ– 16mhz                                          
    CLK->CKDIVR  &= 0XE7;   
    CLK->CKDIVR &= 0xF8;    
    CLK->ICKCR |= CLK_ICKCR_HSION;     
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM4,DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_I2C1,DISABLE);    
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI1,DISABLE);
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
  GPIO_Init(GPIOA,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Slow);
  
}

// Ö¸Ê¾µÆ³õÊ¼»¯
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

// ´¥ÃþIO³õÊ¼»¯£¬ÉÏÉýÑØ´¥·¢
void Init_TOUCHGPIO(void)
{
	GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
	disableInterrupts();
    EXTI_SelectPort(EXTI_Port_B);
	EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Rising);   
    enableInterrupts();                                           //Ê¹ÄÜÖÐ¶Ï
}

void main()
{    

    RCC_Config();
    FreeGPIO_Config();
	Key_GPIO_Init();
	Init_TOUCHGPIO();
	UART_INIT(115200);
 	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);
	Init_LedGPIO();				

    while(1)
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
			case KEY_VAL_AM:	debug("KEY_VAL_AM");
		   break;
			case  KEY_VAL_POW_CA:debug("KEY_VAL_POW_CA");
		   break;
		   break;
			case KEY_VAL_Y30:debug("KEY_VAL_Y30");
		   break;
			case KEY_VAL_I30:debug("KEY_VAL_I30");
		   break;
			case KEY_VAL_I60:debug("KEY_VAL_I60");
		   break;
			case KEY_VAL_I100:debug("KEY_VAL_I100");
		   break;
			case KEY_VAL_MOTZ:debug("KEY_VAL_MOTZ");
		   break;
			case KEY_VAL_MOTY:debug("KEY_VAL_MOTY");
		   break;
			case KEY_VAL_DUIMA:debug("KEY_VAL_DUIMA");
		   break;
		   
		 }
		 keyval = KEY_VAL_NULL;
			debug("\r\n");
	   }	  
	  


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
		//debug("sta in main = 0x%x\r\n",NRF24L01_Read_Reg(STATUS));
		//debug("fer in main = 0x%x\r\n",NRF24L01_Read_Reg(NRF_FIFO_STATUS));
    }   
    
}

//TIM2æ›´æ–°ä¸­æ–­,500ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{

    TIM2->SR1 = (uint8_t)(~(uint8_t)TIM2_IT_Update);//TIM2_ClearITPendingBit(TIM2_IT_Update);     
}


//è‡ªåŠ¨å”¤é†’
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{       
   RTC_ClearITPendingBit(RTC_IT_WUT);  
}


#ifdef USE_FULL_ASSERT
void assert_failed(u8* file,u32 line)
{
  while(1);
}
#endif
           
//´¥ÃþIO
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
  	if(GPIO_READ(TOUCH_IO) != RESET)	debug("in touch TI \r\n");
	
	EXTI_ClearITPendingBit (EXTI_IT_Pin1);  
       
}


//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
   prx.IRQCallBack(&prx);
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}             