#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "lowpower.h"

Nrf24l01_PRXStr prx = {0};		//NRF接收结构体
u8 txbuf[] = {1};				//nrf发送缓存
u8 rxbuf[100] = {0};			//nrf接收缓存

uint16_t BATdat;				//电池电量
u8 flag_wake = 1;				//唤醒标志

//低时钟和GPIO初始化
void CLK_GPIO_Init()
{
    CLK_INIT(HIS_DIV1,CPU_DIV1);                                            
    CLK_HSICmd(ENABLE);   
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

void main()
{
  	CLK_GPIO_Init();										// 低功耗时钟和GPIO初始化
	UART_INIT(115200);
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_GPIO_Init();   										// 双色LED初始化
	MX830Motor_GPIOInit();                                 	// 马达IO配置
	
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();
	
	while(1)
	{
      //休眠函数
      if(flag_wake == 0)
      {
            TIM2_Cmd(DISABLE);
            CLK_LSICmd(ENABLE);											//使能LSI
            CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTC时钟源LSI，1分频=38K
            while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //等待LSI就绪
            RTC_WakeUpCmd(DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTC时钟门控使能
            RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);        
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //开启中断
            RTC_SetWakeUpCounter(2375*time_wakeup);                     //唤醒间隔
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //唤醒定时器中断使能
            RTC_WakeUpCmd(ENABLE);                                      //RTC唤醒使能            
            DATA_Init();               
            halt();
            
            if(flag_exti == 0)Wake_CheckHT();             //定时唤醒函数
            if(flag_exti)               //按键唤醒
            {
                flag_wake = 1;
                //时钟初始化                                           
                CLK->CKDIVR  &= 0XE7;   
                CLK->CKDIVR &= 0xF8;    
                CLK->ICKCR |= CLK_ICKCR_HSION;      // CLK_HSICmd(ENABLE);
                NRF24L01_GPIO_Init();
                NRF24L01_PWR(1);
                FreeNRFGPIO();                
                TIM2_Cmd(ENABLE);
            } 
        
      }
	  else   
	  {
			delay_ms(1000);
			GPIO_RESET(LED1_GREENGPIO);
			GPIO_SET(LED2_REDGPIO);
			delay_ms(1000);
			GPIO_SET(LED1_GREENGPIO);
			GPIO_RESET(LED2_REDGPIO);
			
			/*nrf接收函数*/
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
	  }	    

	}
}

//IRQ 中断服务函数
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	prx.IRQCallBack(&prx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

