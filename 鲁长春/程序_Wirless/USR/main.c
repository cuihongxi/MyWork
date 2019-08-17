/**
	2019-8-16
	

*/
#include "uhead.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_flash.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"


Nrf24l01_PRXStr prx = {0};
u8 txbuf[] = "ABCD";
u8 rxbuf[32] = {0};
//时钟配置
void RCC_Config()
{
   //时钟初始化 16mhz                                          
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

//没有用到的引脚设置低功耗
void FreeGPIO_Config()
{
  GPIO_Init(GPIOA,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOB,GPIO_Pin_7,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOC,GPIO_Pin_2,GPIO_Mode_Out_PP_High_Slow);
  GPIO_Init(GPIOA,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Slow);
  
}


void main()
{    
   
    RCC_Config();
    FreeGPIO_Config();
		
	UART_INIT(115200);
 	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	// 初始化接收模式	
	
    while(1)
    {           
		delay_ms(2000);
		//debug("sta in main = 0x%x\r\n",NRF24L01_Read_Reg(STATUS));
		//debug("fer in main = 0x%x\r\n",NRF24L01_Read_Reg(NRF_FIFO_STATUS));
    }   
    
}

//TIM2更新中断,500ms
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{

    TIM2->SR1 = (uint8_t)(~(uint8_t)TIM2_IT_Update);//TIM2_ClearITPendingBit(TIM2_IT_Update);     
}


//自动唤醒
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
              
//IRQ 中断服务函数
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
   prx.IRQCallBack(&prx);
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}             