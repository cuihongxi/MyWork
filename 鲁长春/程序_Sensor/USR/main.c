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
#include "ADC_CHECK.H"
#include "BAT.H"
#include "FL.H"
#include "LED_SHOW.H"
#include "stmflash.h"
#include "CUI_RTOS.H"
#include "stm8l15x_wwdg.h"

Nrf24l01_PTXStr 	ptx 		= {0};				// NRF发送结构体

u8      TXrxbuf[7] = {0};
u8      TXtxbuf[7] = {0};
addrNRFStr		addrNRF = {0};

u8      nrfaddr[5];
u32 	systime = 0;
u32 	DM_time = 0;
u8		flag_duima = 0;
u8      DM_num = 0;
u8		flag_exti = 0;
u16 	YS_CGdat  = 10000;
u8		CGDAT[5] = {0};
u8		flag_open = 0;

extern 	u8		flag_FL_SHUT;	// FL关窗标志  
extern 	u8		flag_FLCheckState;
#define			KEY_DM				GPIOB,GPIO_Pin_2

TaskStr* 		taskBatControl 	= {0};	
TaskStr* 		taskYS		= {0};				    // YS测量任务
TaskLinkStr		task_link 	= {0};
TimerLinkStr 	timer2 		= {0};					// 任务的定时器
TaskLinkStr* 	tasklink 	= &task_link;			// 任务列表
BATStr 			bat = {0};					        // 电池结构体
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
void NRF_SendDMCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
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
	LEN_GREEN_Open();
	ptx->index = 0;
	debug("本地地址：");
	u8 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx->txbuf[i]);
	}
	debug("\r\n");
	
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
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
	
	debug("数据：");
	u8 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx->txbuf[i]);
	}
	debug("\r\n");

	for(i=0;i<8;i++)
	{
		if(addrNRF.index&(1<<i))ptx->index ++;
		else break;
	}
	debug("addrNRF.index = %d, 总共配对数：%d\r\n",addrNRF.index,ptx->index);
	if(ptx->index > 0) 
	{
	  	LEN_GREEN_Open();
	  	ptx->index --;
		for(i =0;i<5;i++)
		{
			debug("%X	",addrNRF.addr[ptx->index][i]);
		}
		debug("\r\n");	
		
		//NRF24L01_TX_Mode(addrNRF.addr[ptx->index]);
		InitNRF_AutoAck_PTX(ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,addrNRF.addr[ptx->index]);
		NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
	}
    
  
}

void Key_GPIO_Init()
{
	GPIO_Init(KEY_DM,GPIO_Mode_In_PU_IT);
    flag_exti = 0;
}

void Key_Open()
{
    if(GPIO_READ(KEY_DM)) //无按键按下
    {       
		if((systime - DM_time) > TIM_CLOSE)
		{
			debug("开机\r\n");
			flag_open = 1;
			
		}
	//	debug("systime = %lu,DM_time = %lu\r\n",systime,DM_time);
		LEN_RED_Close();
		DM_time = 0;
		flag_exti = 0;
    }
}
//松手程序
void Key_ScanLeave()
{
    if(GPIO_READ(KEY_DM)) //无按键按下
    {       
	   	if((systime - DM_time) > TIM_CLEARDM)	
		{
			debug("清除DM信息\r\n");
			FlashClearDM(&addrNRF);
		}else
		if((systime - DM_time) > TIM_CLOSE)
		{
			debug("关机\r\n");
			flag_open = 0;
			WWDG_SWReset();	// 复位
		}else
		if((systime - DM_time) > 500)
		{
		  	debug("DM\r\n");
			flag_duima = 1;
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);	
		}
		LEN_GREEN_Close();

		DM_time = 0;
		flag_exti = 0;
    }
}

void Key_Scan()
{
	if(GPIO_READ(KEY_DM) == 0)
	{
	  debug("systime = %lu\r\n",systime);
		DM_time = systime;
		flag_exti = 1;
		if(flag_open)
			LEN_GREEN_Open();
		else LEN_RED_Open();
		  
	} 
}

// 装载NRF发射数据，前两位装载YS的AD值，再装载FL关闭标志，再装载电池电量标志
void LoadingNRFData(u8* pBuf,u16 YSadc,u8 FLflag,u8 batState)
{
	*(u16*)pBuf = YSadc;
	pBuf[2] = FLflag;
	pBuf[3] = batState;
}

void ReturnADDRESS2()
{
  //NRF24L01_TX_Mode(ADDRESS2);
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
}

// 显示全部已保存的地址
void ShowAllAddr(addrNRFStr* buf)
{
  	u8 i;
	for(i=0;i<8;i++)
	{
		if(buf->index&(1<<i))
		{
			debug("配对号 %d : ",i);	
			u8 j = 0;
			for(j =0;j<5;j++)
			{
				debug("%X	",buf->addr[i][j]);
			}
			debug("\r\n");
		}
		else break;
	}	
}
void main()
{    	
	UART_INIT(115200);	
	Key_GPIO_Init();
	LED_GPIO_Init();
	debug("start:\r\n");
	FlashData_Init(&addrNRF);
	ShowAllAddr(&addrNRF);
	NRF_CreatNewAddr(ADDRESS2);
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
    ptx.rxbuf = TXrxbuf;
	ptx.txbuf = TXtxbuf;
	debug("本地地址：");
	u16 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx.txaddr[i]);
	}
	debug("\r\n");
	
    NRF24L01_GPIO_Lowpower();
	Make_SysSleep();
	while(flag_open == 0)
	{
	  	halt();	
		if(flag_exti) Key_Open();
	}
	taskBatControl = OS_CreatTask(&timer2);			// 创建电池电量检测任务
	taskYS = OS_CreatTask(&timer2);					// 创建YS测量任务 ，每2秒检测一次
	OS_AddFunction(taskYS,YS_Function,TIM_CHECKEYS);
	OS_AddTask(tasklink,taskYS);								// 添加YS检测任务
	
	//检测一次电池电压
	GPIO_ADC_Init();
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	debug("bat = %d.%d\r\n",(u8)bat.val,(u8)(bat.val*10)-(u8)bat.val*10);
	FL_GPIO_Init();

    while(1)
    {    
      halt();	

	   if(flag_duima == 0)			// 非对码状态
	   {
		 	  //按键检测
	   		if(flag_exti) Key_ScanLeave();
				  systime = OS_TimerFunc(&timer2);			// OS定时器内函数，获得系统时间
			  if(systime >= bat.threshold) bat.flag = 1;// 电池电量检测间隔	  
			  BatControl(&bat,tasklink,taskBatControl);
			  OS_Task_Run(tasklink);				// 执行任务链表中的任务
			 if(flag_FLCheckState == 0)
			 {
			 	static u32 t = 0;
				t += IRQ_PERIOD;
				if(t > TIM_FL)
				{
					t = 0;
					FL_CheckStart();
				}
			 }
	   }else
	   {
		// debug("DM 模式\r\n");
		 static u8 time = 0;
		 if((time & 0x0f) == 0 )
		 {
		 	NRF_SendDMCMD(&ptx,ADDRESS2,CMD_DM,MES_DM);	
		 }
		 time ++;
		 if(time>250)
		 {
		 	flag_duima = 0;
			time = 0;
			LEN_GREEN_Close();
			NRF24L01_PWR(0);
		 } 
	   }

        
	}   
    
}
 

//自动唤醒
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
  if(flag_open == 0)systime += IRQ_PERIOD;
   	RTC_ClearITPendingBit(RTC_IT_WUT);  
}

//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
  if(GPIO_READ(NRF24L01_IRQ_PIN)== RESET)
  {	
 		ptx.IRQCallBack(&ptx);
  }
	Key_Scan();
   EXTI_ClearITPendingBit (EXTI_IT_Pin2);
} 
