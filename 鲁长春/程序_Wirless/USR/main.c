/**
	2019-8-16
	ң��������

*/
#include "uhead.h"
#include "MYCLK.h"
#include "stm8l15x_flash.h"
#include "24l01.h"
#include "NRF24L01_AUTO_ACK.H"
#include "keyboard.h"
#include "stmflash.h"
#include "UDATA.H"
#include "stm8l15x_rtc.h"
#include "stm8l15x_iwdg.h"
#include "stm8l15x_itc.h"

#define		PRESS_Y30	0X01
#define		PRESS_MOTZ	0X02
#define		PRESS_MOTY	0X04

#define	        TIM_MAXDELAY	2000

#define         IRQ_PERIOD      500   

Nrf24l01_PRXStr 	prx 		= {0};				// NRF���սṹ��
Nrf24l01_PTXStr 	ptx 		= {0};				// NRF���ͽṹ��

u8      RXrxbuf[7] = {0};
u8      RXtxbuf[7] = {0};
u8      TXrxbuf[7] = {0};
u8      TXtxbuf[7] = {0};

u8 		keyval = 0;
u8 		flag_wake = 1;
u32 	systime = 0;
u8 		pressKey = 0;
u32 	presstime = 0;
u8		is_DM = 0;		// �����Ƿ������Ϣ
u8		flag_duima = 0;
u8      DM_num = 0;
u8		LEDtimes = 0;
u32		sendtime = 0;

extern 		u32 		DM_time;


void RCC_Config()
{
  //CLK_INIT(HIS_DIV1,CPU_DIV1);
  CLK_Change2HSI();				//�л�HSIʱ�� 
}


// ָʾ�Ƴ�ʼ��
void Init_LedGPIO(void)
{
	//GPIO_Init(Z_LED,GPIO_Mode_Out_PP_High_Slow);
	GPIO_Init(T_LED,GPIO_Mode_Out_PP_High_Slow);
	//GPIO_RESET(Z_LED);
	GPIO_RESET(T_LED);
	delay_ms(1000);
	//GPIO_SET(Z_LED);
	GPIO_SET(T_LED);
	  
}

//��ϵͳ����
void Make_SysSleep()
{
    	//debug(" Sleep \r\n");

	NRF24L01_PWR(0); 						// �ر�NRF�ĵ�Դ

	CLK_LSICmd(ENABLE);						// ʹ��LSI
	CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);  	// RTCʱ��ԴLSI��
	while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);        	// �ȴ�LSI����
	RTC_WakeUpCmd(DISABLE);
	CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);      	// RTCʱ���ſ�ʹ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div2);   		// 19Kʱ��Ƶ��
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// �����ж�
	RTC_SetWakeUpCounter(9500);                     		// ���Ѽ��	500mS
	RTC_ITConfig(RTC_IT_WUT, ENABLE);                           	// ���Ѷ�ʱ���ж�ʹ��
	RTC_WakeUpCmd(ENABLE);                                      	// RTC����ʹ��  
	PWR_UltraLowPowerCmd(ENABLE); 					// ʹ�ܵ�Դ�ĵ͹���ģʽ
	PWR_FastWakeUpCmd(ENABLE);
}

//ͨ��NRF�����巢�������
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
{
    NRF24L01_PWR(1);
	NRF24L01_ClearFIFO();
    ptx->txbuf[0] = addr[0];
    ptx->txbuf[1] = addr[1];
    ptx->txbuf[2] = addr[2];
    ptx->txbuf[3] = addr[3];
    ptx->txbuf[4] = addr[4];
    ptx->txbuf[5] = cmd;
    ptx->txbuf[6] = mes;
	sendtime = systime;
	GPIO_RESET(T_LED);
    NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}

//�����ַ��flash
void SaveFlashAddr(u8* buf)
{
  #if  DEBUG_LEVEL == 0
  ADDRESS2[0] = buf[0];
  ADDRESS2[1] = buf[1];
  ADDRESS2[2] = buf[2];
  ADDRESS2[3] = buf[3];
  ADDRESS2[4] = buf[4];

  FLASH_ProgramByte(EEPROM_ADDRESS0,ADDRESS2[0]);
  FLASH_ProgramByte(EEPROM_ADDRESS1,ADDRESS2[1]);
  FLASH_ProgramByte(EEPROM_ADDRESS2,ADDRESS2[2]);
  FLASH_ProgramByte(EEPROM_ADDRESS3,ADDRESS2[3]);
  FLASH_ProgramByte(EEPROM_ADDRESS4,ADDRESS2[4]);
#endif
}

// ���DM
void ClearDM()
{
     // debug("clear DM \r\n");
  #if  DEBUG_LEVEL == 0

    FLASH_ProgramByte(EEPROM_ADDRESS0,ADDRESS1[0]);
    FLASH_ProgramByte(EEPROM_ADDRESS1,ADDRESS1[1]);
    FLASH_ProgramByte(EEPROM_ADDRESS2,ADDRESS1[2]);
    FLASH_ProgramByte(EEPROM_ADDRESS3,ADDRESS1[3]);
    FLASH_ProgramByte(EEPROM_ADDRESS4,ADDRESS1[4]);
    ADDRESS2[0] = ADDRESS1[0];
    ADDRESS2[1] = ADDRESS1[1];
    ADDRESS2[2] = ADDRESS1[2];
    ADDRESS2[3] = ADDRESS1[3];
    ADDRESS2[4] = ADDRESS1[4];
    address = ADDRESS2;
    InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ);
    NRF24L01_GPIO_Lowpower();
#endif
}
//DMģʽ�Զ�������ɻص�����
void dmRXD_CallBack(Nrf24l01_PRXStr* prx) 
{
      prx->rxlen = NRF24L01_GetRXLen();	
      NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf,prx->rxlen);	//��ȡ����   	
      NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// ���RX_DS�жϱ�־
  
      if(prx->rxbuf[5] == CMD_DM && prx->rxbuf[6] == MES_DM) 
      { 
            SaveFlashAddr(prx->rxbuf);
            flag_duima = 0;
           // debug("\r\n---DM���---\r\n");
      }
      	if(prx->rxlen)
	{
		prx->rxlen = 0;
	}
}
void DM_Mode()
{
      flag_duima = 1;
      address = ADDRESS1;
      InitNRF_AutoAck_PRX(&prx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP0,RF_CH_HZ);	
      
      NRF24L01_PWR(1);
      prx.txbuf[0] = ADDRESS3[0];
      prx.txbuf[1] = ADDRESS3[1];
      prx.txbuf[2] = ADDRESS3[2];
      prx.txbuf[3] = ADDRESS3[3];
      prx.txbuf[4] = ADDRESS3[4];
      prx.txbuf[5] = 'D';
      prx.txbuf[6] = 'M';
      
      NRF24L01_RX_AtuoACKPip(prx.txbuf,7,prx.pip);	//���Ӧ���ź�
      prx.RXDCallBack = dmRXD_CallBack;
      DM_num = DM_NUM;
      
      while(flag_duima);
      address = ADDRESS2;
	  InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ);
      NRF24L01_GPIO_Lowpower();     
}


// ����IO��ʼ���������ش���
void Init_TOUCHGPIO(void)
{
	GPIO_Init(TOUCH_IO,GPIO_MODE_TOUCH);
	disableInterrupts();
    EXTI_SelectPort(EXTI_Port_B);
	EXTI_SetPinSensitivity(EXTI_Pin_5,EXTI_Trigger_Rising);   
	GPIO_RESET(TOUCH_IO);
    enableInterrupts();                                           //ʹ���ж�
}

void main()
{    

	Key_GPIO_Init();
	UART_INIT(115200);	
//#if  DEBUG_LEVEL == 0
    FlashData_Init();
    Init_LedGPIO();    
//#endif
	
    address = ADDRESS2;
	InitNRF_AutoAck_PTX(&ptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ);
    ptx.rxbuf = TXrxbuf;
		
	NRF_CreatNewAddr(ADDRESS3);
    NRF24L01_GPIO_Lowpower();
	Make_SysSleep();
	
//	IWDG_Enable();
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
//	IWDG_SetPrescaler(IWDG_Prescaler_256);
//	IWDG_SetReload(0xFF);
//    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	//Init_TOUCHGPIO();

    while(1)
    {    
        halt();
		
	  //�������
	   if(flag_exti) Key_ScanLeave();
	   if(flag_touch)Key_TouchtLeave();
	   if(keyval != KEY_VAL_NULL && keyval != KEY_VAL_DUIMA && keyval != KEY_VAL_AM && keyval != KEY_VAL_POW_CA)
	   {
		 switch(keyval)
		 {
		     case KEY_VAL_Y30: pressKey = PRESS_Y30;presstime = systime + TIM_MAXDELAY;
		   break;
		     case KEY_VAL_I30:
                       if(systime<=presstime)
                       {
                         switch(pressKey){
                             case PRESS_Y30:	//debug("��ʱ30����");
							 	NRF_SendCMD(&ptx,ADDRESS3,CMD_Y30, MES_Y30_3_1);
                             break;
                             case PRESS_MOTZ:	//debug("����1/3");
							 	NRF_SendCMD(&ptx,ADDRESS3,CMD_Z, MES_Z_3_1);
                             break;
                             case PRESS_MOTY:	//debug("��￪1/3");
							 	NRF_SendCMD(&ptx,ADDRESS3,CMD_Y, MES_Y_3_1);
                             break;
                         };                      
                       }
                     	presstime = 0;
		     			pressKey = 0;
		   break;
			case KEY_VAL_I60:
                        if(systime<=presstime)
                       {                         
						switch(pressKey){
						 case PRESS_Y30://debug("��ʱ60����");
						   NRF_SendCMD(&ptx,ADDRESS3,CMD_Y30, MES_Y30_3_2);
						 break;
						 case PRESS_MOTZ://debug("����2/3");
						   NRF_SendCMD(&ptx,ADDRESS3,CMD_Z, MES_Z_3_2);
						 break;
						 case PRESS_MOTY://debug("��￪2/3");
						   NRF_SendCMD(&ptx,ADDRESS3,CMD_Y, MES_Y_3_2);
						 break;
                        };
                       }
                     	presstime = 0;
		     			pressKey = 0;
		   break;
			case KEY_VAL_I100:
                       if(systime <= presstime)
                       {
                         
								switch(pressKey){
                                 case PRESS_Y30://debug("��ʱ90����");
								   NRF_SendCMD(&ptx,ADDRESS3,CMD_Y30, MES_Y30_3_3);
                                 break;
                                 case PRESS_MOTZ://debug("���ȫ��");
								   NRF_SendCMD(&ptx,ADDRESS3,CMD_Z, MES_Z_3_3);
                                 break;
                                 case PRESS_MOTY://debug("���ȫ��");
								   NRF_SendCMD(&ptx,ADDRESS3,CMD_Y, MES_Y_3_3);
                                 break;
                            	};
                       }
                     	presstime = 0;
		     			pressKey = 0;
		   break;
			case KEY_VAL_MOTZ:	pressKey = PRESS_MOTZ;
								presstime = systime + TIM_MAXDELAY;
								NRF_SendCMD(&ptx,ADDRESS3,CMD_Z, MES_Z);
		   break;
			case KEY_VAL_MOTY:	pressKey = PRESS_MOTY;
								presstime = systime + TIM_MAXDELAY;
								NRF_SendCMD(&ptx,ADDRESS3,CMD_Y, MES_Y);
		   break;
		   
		 }
		 if(keyval != KEY_VAL_Y30 && keyval != KEY_VAL_I30) keyval = KEY_VAL_NULL;
		 
		 
		
		
	   }	  		
    
	
	}   
    
}
 

//�Զ�����
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
        systime += IRQ_PERIOD;
        if(flag_duima)
        {
          DM_num--;
          if(DM_num == 0)flag_duima = 0;
        }
		//IWDG_ReloadCounter() ;
   	RTC_ClearITPendingBit(RTC_IT_WUT);  
}

//NRF24L01 IRQ 
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
  if(GPIO_READ(NRF24L01_IRQ_PIN)== RESET)
  {	
      if(flag_duima == 0)	
        ptx.IRQCallBack(&ptx);
      else
        prx.IRQCallBack(&prx);
  }
   EXTI_ClearITPendingBit (EXTI_IT_Pin4);
}             