#include "uhead.h"
#include "MX830Motor.h"
#include "LED_SHOW.H"
#include "MYCLK.h"
#include "ADC_CHECK.H"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "lowpower.h"

Nrf24l01_PRXStr prx = {0};		//NRF���սṹ��
u8 txbuf[] = {1};				//nrf���ͻ���
u8 rxbuf[100] = {0};			//nrf���ջ���

uint16_t BATdat;				//��ص���
u8 flag_wake = 1;				//���ѱ�־

//��ʱ�Ӻ�GPIO��ʼ��
void CLK_GPIO_Init()
{
    CLK_INIT(HIS_DIV1,CPU_DIV1);                                            
    CLK_HSICmd(ENABLE);   
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

void main()
{
  	CLK_GPIO_Init();										// �͹���ʱ�Ӻ�GPIO��ʼ��
	UART_INIT(115200);
	InitNRF_AutoAck_PRX(&prx,rxbuf,txbuf,sizeof(txbuf),BIT_PIP0,RF_CH_HZ);	
	LED_GPIO_Init();   										// ˫ɫLED��ʼ��
	MX830Motor_GPIOInit();                                 	// ���IO����
	
	GPIO_Init(YSD_GPIO,GPIO_Mode_Out_PP_High_Slow);
	GPIO_ADC_Init();
	
	while(1)
	{
      //���ߺ���
      if(flag_wake == 0)
      {
            TIM2_Cmd(DISABLE);
            CLK_LSICmd(ENABLE);											//ʹ��LSI
            CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  //RTCʱ��ԴLSI��1��Ƶ=38K
            while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        //�ȴ�LSI����
            RTC_WakeUpCmd(DISABLE);
            CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      //RTCʱ���ſ�ʹ��
            RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);        
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //�����ж�
            RTC_SetWakeUpCounter(2375*time_wakeup);                     //���Ѽ��
            RTC_ITConfig(RTC_IT_WUT, ENABLE);                           //���Ѷ�ʱ���ж�ʹ��
            RTC_WakeUpCmd(ENABLE);                                      //RTC����ʹ��            
            DATA_Init();               
            halt();
            
            if(flag_exti == 0)Wake_CheckHT();             //��ʱ���Ѻ���
            if(flag_exti)               //��������
            {
                flag_wake = 1;
                //ʱ�ӳ�ʼ��                                           
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
			
			/*nrf���պ���*/
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

//IRQ �жϷ�����
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
	prx.IRQCallBack(&prx);
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}

