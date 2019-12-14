/**
	2019年11月18日
	无线传感器程序

*/
#include "uhead.h"
#include "MYCLK.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "UDATA.H"
#include "stm8l15x_rtc.h"
#include "stm8l15x_itc.h"
u8  		ADDRESS4[RX_ADR_WIDTH]={2,2,2,2,2};  		//与传感器通讯地址 
Nrf24l01_PTXStr 	ptx 		= {0};				// NRF发送结构体

u8      TXrxbuf[7] = {0};
u8      TXtxbuf[7] = {0};

u8      nrfaddr[5];
u32 	systime = 0;
u32 	DM_time = 0;
u8		flag_duima = 0;
u8      DM_num = 0;
u8		flag_exti = 0;

extern 		u32 		DM_time;
#define         LED_RED            	GPIOC,GPIO_Pin_4         //红灯
#define         LED_GREEN           GPIOB,GPIO_Pin_1         

#define			KEY_DM				GPIOC,GPIO_Pin_5

void LEN_RED_Open()
{
	GPIO_SET(LED_RED);
}

void LEN_RED_Close()
{
	GPIO_RESET(LED_RED);
}	
void LEN_GREEN_Open()
{
	GPIO_RESET(LED_GREEN);
}

void LEN_GREEN_Close()
{
	GPIO_SET(LED_GREEN);
}
//双色LED初始化
void LED_GPIO_Init()
{
    GPIO_Init(LED_RED,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(LED_GREEN,GPIO_Mode_Out_OD_HiZ_Slow);
	LEN_GREEN_Open();
	LEN_RED_Open();
	delay_ms(1000);
	LEN_RED_Close();
	LEN_GREEN_Close();
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
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   			// 19K时钟频率
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 开启中断
	RTC_SetWakeUpCounter(9500);                     				// 唤醒间隔	500mS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// 唤醒定时器中断使能
	RTC_WakeUpCmd(ENABLE);                                      	// RTC唤醒使能  
	PWR_UltraLowPowerCmd(ENABLE); 									// 使能电源的低功耗模式
	PWR_FastWakeUpCmd(ENABLE);
}

//通过NRF向主板发送命令函数
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
{
    NRF24L01_PWR(1);
	NRF24L01_ClearTXFIFO();
    ptx->txbuf[0] = addr[0];
    ptx->txbuf[1] = addr[1];
    ptx->txbuf[2] = addr[2];
    ptx->txbuf[3] = addr[3];
    ptx->txbuf[4] = addr[4];
    ptx->txbuf[5] = cmd;
    ptx->txbuf[6] = mes;
	LEN_RED_Open();
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}

void Key_GPIO_Init()
{
	GPIO_Init(KEY_DM,GPIO_Mode_In_PU_No_IT);
    flag_exti = 0;
}

//松手程序
void Key_ScanLeave()
{
    if(GPIO_READ(KEY_DM)) //无按键按下
    {       
	   LEN_GREEN_Close();
		if((systime - DM_time)< 2000)
		{
				flag_duima = 1;
				
		}
		DM_time = 0;
		flag_exti = 0;
    }
}

void Key_Scan()
{
	if(GPIO_READ(KEY_DM) == 0)
	{
		DM_time = systime;
		flag_exti = 1;
		LEN_GREEN_Open();
	} 
}
void main()
{    	
	UART_INIT(115200);	
	Key_GPIO_Init();
	LED_GPIO_Init();
	
	NRF_CreatNewAddr(ADDRESS2);
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP1,RF_CH_HZ,ADDRESS2);
    ptx.rxbuf = TXrxbuf;
	debug("当前的通讯地址：");
	u8 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx.txaddr[i]);
	}
	debug("\r\n");
	
    NRF24L01_GPIO_Lowpower();
	Make_SysSleep();

    while(1)
    {    
       halt();	
//	   static u8 i = 0;
//	   i = !i;
//	   if(i) LEN_RED_Close();
//	   else LEN_RED_Open();
	   if(flag_duima == 0)			// 非对码状态
	   {
		 	  //按键检测
	   		if(flag_exti) Key_ScanLeave();
			else Key_Scan();
	   }else
	   {
			debug("DM 模式\r\n");
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);			  	
			NRF_SendCMD(&ptx,ADDRESS2,CMD_DM,MES_DM);			
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP1,RF_CH_HZ,ADDRESS2);
			flag_duima = 0;
			LEN_RED_Close();
	   }

        
	}   
    
}
 

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    systime += IRQ_PERIOD;
   	RTC_ClearITPendingBit(RTC_IT_WUT);  
}

//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
  if(GPIO_READ(NRF24L01_IRQ_PIN)== RESET)
  {	
 		ptx.IRQCallBack(&ptx);
  }
   EXTI_ClearITPendingBit (EXTI_IT_Pin0);
} 
