/**
	2019��11��18��
	���ߴ���������

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

Nrf24l01_PTXStr 	ptx 		= {0};				// NRF���ͽṹ��

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

extern 	u8			flag_FL_SHUT;	// FL�ش���־  

#define			KEY_DM				GPIOB,GPIO_Pin_2

TaskStr* 		taskBatControl 	= {0};	
TaskStr* 		taskYS		= {0};				    // YS��������
TaskLinkStr		task_link 	= {0};
TimerLinkStr 	timer2 		= {0};					// ����Ķ�ʱ��
TaskLinkStr* 	tasklink 	= &task_link;			// �����б�
BATStr 			bat = {0};					        // ��ؽṹ��
//��ϵͳ����
void Make_SysSleep()
{
	NRF24L01_PWR(0); 						// �ر�NRF�ĵ�Դ

	CLK_LSICmd(ENABLE);						// ʹ��LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTCʱ��ԴLSI��
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// �ȴ�LSI����
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTCʱ���ſ�ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   			// 19Kʱ��Ƶ��
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// �����ж�
	RTC_SetWakeUpCounter(9500);                     				// ���Ѽ��	500mS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// ���Ѷ�ʱ���ж�ʹ��
	RTC_WakeUpCmd(ENABLE);                                      	// RTC����ʹ��  
	PWR_UltraLowPowerCmd(ENABLE); 									// ʹ�ܵ�Դ�ĵ͹���ģʽ
	PWR_FastWakeUpCmd(ENABLE);
}

//ͨ��NRF�����巢�������
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
	LEN_GREEN_Open();
	
	debug("���ݣ�");
	u8 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx->txbuf[i]);
	}
	debug("\r\n");
	
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}

void Key_GPIO_Init()
{
	GPIO_Init(KEY_DM,GPIO_Mode_In_PU_IT);
    flag_exti = 0;
}

//���ֳ���
void Key_ScanLeave()
{
    if(GPIO_READ(KEY_DM)) //�ް�������
    {       
	   	if((systime - DM_time) > 8000)	
		{
			debug("���DM��Ϣ\r\n");
		}else
		if((systime - DM_time) > 500)
		{
		  	debug("DM\r\n");
			flag_duima = 1;
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);	
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
		
	} 
}

// װ��NRF�������ݣ�ǰ��λװ��YS��ADֵ����װ��FL�رձ�־����װ�ص�ص�����־
void LoadingNRFData(u8* pBuf,u16 YSadc,u8 FLflag,u8 batState)
{
	*(u16*)pBuf = YSadc;
	pBuf[2] = FLflag;
	pBuf[3] = batState;
}

void ReturnADDRESS2()
{
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
}
void main()
{    	
	UART_INIT(115200);	
	Key_GPIO_Init();
	LED_GPIO_Init();
	debug("start:\r\n");
	FlashData_Init(addrNRF);
	
	NRF_CreatNewAddr(ADDRESS2);
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);
    ptx.rxbuf = TXrxbuf;
	ptx.txbuf = TXtxbuf;
	debug("��ǰ��ͨѶ��ַ��");
	u16 i = 0;
	for(i =0;i<5;i++)
	{
		debug("%X	",ptx.txaddr[i]);
	}
	debug("\r\n");
    NRF24L01_GPIO_Lowpower();
	
	taskBatControl = OS_CreatTask(&timer2);			// ������ص����������
	taskYS = OS_CreatTask(&timer2);					// ����YS�������� ��ÿ2����һ��
	OS_AddFunction(taskYS,YS_Function,TIM_CHECKEYS);
	OS_AddTask(tasklink,taskYS);								// ���YS�������
	
	//���һ�ε�ص�ѹ
	GPIO_ADC_Init();
	bat.flag= 1;
	BatControl(&bat,tasklink,taskBatControl);
	debug("bat = %d.%d\r\n",(u8)bat.val,(u8)(bat.val*10)-(u8)bat.val*10);
	FL_GPIO_Init();
	Make_SysSleep();
	
	
	
    while(1)
    {    
      halt();	
	  systime = OS_TimerFunc(&timer2);			// OS��ʱ���ں��������ϵͳʱ��
	  if(systime >= bat.threshold) bat.flag = 1;// ��ص��������	  
	  BatControl(&bat,tasklink,taskBatControl);
	  
	  OS_Task_Run(tasklink);				// ִ�����������е�����
	   if(flag_duima == 0)			// �Ƕ���״̬
	   {
		 	  //�������
	   		if(flag_exti) Key_ScanLeave();
			
	   }else
	   {
		// debug("DM ģʽ\r\n");
		 static u8 time = 0;
		 if((time & 0x0f) == 0)
		 {
		 	NRF_SendCMD(&ptx,ADDRESS2,CMD_DM,MES_DM);	
		 }
		 time ++;
			
	   }

        
	}   
    
}
 

//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
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
