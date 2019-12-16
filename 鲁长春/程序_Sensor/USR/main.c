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
#include "stm8l15x_flash.h"


Nrf24l01_PTXStr 	ptx 		= {0};				// NRF���ͽṹ��

u8      TXrxbuf[7] = {0};
u8      TXtxbuf[7] = {0};

u8      nrfaddr[5];
u32 	systime = 0;
u32 	DM_time = 0;
u8		flag_duima = 0;
u8      DM_num = 0;
u8		flag_exti = 0;
u16 	YS_CGdat  = 10000;
u8		CGDAT[4] = {0};

JugeCStr 		jugeYSADC 	= {0};	// YS ADC��� 

extern u8		flag_FL_SHUT;	// FL�ش���־
	  
#if  DEBUG_LEVEL > 0
#define			KEY_DM				GPIOB,GPIO_Pin_1
#else
#define			KEY_DM				GPIOC,GPIO_Pin_5
#endif

u32 	GetSysTime()
{
	return systime;
}
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
//	LEN_RED_Open();
	
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
	GPIO_Init(KEY_DM,GPIO_Mode_Out_OD_HiZ_Slow);
    flag_exti = 0;
}

//���ֳ���
void Key_ScanLeave()
{
    if(GPIO_READ(KEY_DM)) //�ް�������
    {       
	   LEN_RED_Close();
		if((systime - DM_time) > 1000)
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
		LEN_RED_Open();
	} 
}

// װ��NRF��������
void LoadingNRFData(u8* pBuf,u16 YSadc,u8 FLflag)
{
	*(u16*)pBuf = YSadc;
	pBuf[2] = FLflag;
}

void main()
{    	
	UART_INIT(115200);	
	Key_GPIO_Init();
	LED_GPIO_Init();
	
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
	Make_SysSleep();
	jugeYSADC.start = 1;
	
	FLASH_Unlock(FLASH_MemType_Data); 							// ����EEPROM

    while(1)
    {    
      halt();	
	   if(flag_duima == 0)			// �Ƕ���״̬
	   {
		 	  //�������
	   		if(flag_exti) Key_ScanLeave();
			else Key_Scan();
			delay_ms(4000);

	  		LoadingNRFData(CGDAT,YS_CGdat,flag_FL_SHUT);
			NRF_SendCMD(&ptx,CGDAT,CMD_CG,CMD_CG);
//			if(Juge_counter(&jugeYSADC,TIM_YS_ADC)) 
//			{
//			  	GPIO_SET(YSD_GPIO);
//				YS_CGdat = Get_ADC_Dat(YS_Channel);
//				GPIO_RESET(YSD_GPIO);
//				if(YSGetAD(YS_CGdat) > VALVE_YS_D) // ������ֵ
//				{
//					static u32 ys_time = 0;
//					if((systime - ys_time) < TIM__YS_D)	// С���˲����
//					{
//							// ����YS�ź�
//						  LoadingNRFData(CGDAT,YS_CGdat,flag_FL_SHUT);
//						  NRF_SendCMD(&ptx,CGDAT,CMD_CG,CMD_CG);
//					}
//					
//					ys_time = systime;
//				}
//				
//					
//				debug("\r\n");
//			}
	   }else
	   {
			debug("DM ģʽ\r\n");
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);			  	
			NRF_SendCMD(&ptx,ADDRESS2,CMD_DM,MES_DM);			
			InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP1,RF_CH_HZ,ADDRESS2);
			flag_duima = 0;
			LEN_RED_Close();
	   }

        
	}   
    
}
 

//�Զ�����
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
