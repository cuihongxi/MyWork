#include "NRF_App.h"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "CUI_RTOS.H"
#include "LED_SHOW.H"
#include "stmflash.h"
Nrf24l01_PRXStr 	RXprx 		= {0};				//
u8 			RXtxbuf[7] 	= {0,0,0,0,0,'O','K'};		//
u8 			RXrxbuf[7] 	= {0};				        //
u8          DM_num = 0;

extern  u8 		            flag_duima  		;	// 对码标志
extern  TaskStr* 	        taskNRF	            ;
extern  TaskLinkStr* 		tasklink            ;
extern  JugeCStr 		    NRFpowon 	        ;
extern  JugeCStr 		    NRFpowoff 	        ;
extern  JugeCStr 			NRFsleep;
extern 	u16					nrf_sleeptime 	;
extern 	u16					nrf_worktime	;
void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);
void NRFReceived();
bool JugeRX();
void NRF_DM();
bool JugeDM();
void RXD_CallBack(Nrf24l01_PRXStr* prx) ;   // 普通模式接收完成回掉函数
void DMRXD_CallBack(Nrf24l01_PRXStr* prx);	// DM模式接收完成回调函数

/*nrf处理函数*/
void NRF_Function()
{   
        if(taskNRF->state == Stop)
        {
          if(flag_duima == 0)
          {
			debug("本地地址：");
			u8 i = 0;
			for(i =0;i<5;i++)
			{
				debug("%X	",ADDRESS3[i]);
			}
			debug("\r\n");	
			
            InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);	//配置接收模式		
			debug("当前的通讯地址：");
			for(i =0;i<5;i++)
			{
				debug("%X	",ADDRESS2[i]);
			}
			debug("\r\n");
			 InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP1,RF_CH_HZ,ADDRESS4);	//配置接收模式
			debug("传感器通讯地址：");
			for(i =0;i<5;i++)
			{
				debug("%X	",ADDRESS4[i]);
			}
			debug("\r\n");
            RXprx.rxbuf = RXrxbuf;

            NRFpowon.start = 1;
            NRF24L01_PWR(0);
            RXprx.RXDCallBack = RXD_CallBack;
            OS_AddJudegeFunction(taskNRF,NRFReceived,100,JugeRX);
		  }
          else
          {		
              NRFpowon.start = 0;
              NRFpowoff.start = 0;
              InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS1);	//配置接收模式
			  RXprx.txbuf[0] = ADDRESS3[0];
			  RXprx.txbuf[1] = ADDRESS3[1];
			  RXprx.txbuf[2] = ADDRESS3[2];
			  RXprx.txbuf[3] = ADDRESS3[3];
			  RXprx.txbuf[4] = ADDRESS3[4];
			  RXprx.txbuf[5] = 'D';
			  RXprx.txbuf[6] = 'M';
      		  NRF24L01_RX_AtuoACKPip(RXprx.txbuf,7,BIT_PIP0);	//填充应答信号
			  //NRF24L01_RX_AtuoACKPip(i,6,0);	//填充应答信号
              RXprx.RXDCallBack =  DMRXD_CallBack;
			  RXprx.rxbuf = RXrxbuf;
			  
              DM_num = DM_NUM;
			  NRF24L01_PWR(1);
			  NRFpowon.start = 0;
			  NRFpowon.counter = 0;
			  NRFpowoff.start = 0;
			  NRFpowoff.counter = 0;
              OS_AddJudegeFunction(taskNRF,NRF_DM,1000,JugeDM);
          }
           OS_AddTask(tasklink,taskNRF);	
        }
}


void ChangeNRFCmd(u8* buf);
// 主循环中运行的，显示接收到的数据
void NRFReceived()
{
	if(RXprx.hasrxlen)
	{
		//根据收到的字节，映射出相应的控制命令
	    NRF24L01_ClearFIFO();
		nrf_sleeptime = GZ_SLEEP_TIME;
		nrf_worktime = GZ_WORK_TIME;
		NRFsleep.start = 1;
		NRFsleep.counter = 0;
		ChangeNRFCmd(RXprx.rxbuf);
		RXprx.hasrxlen = 0;
//		u8 i[6] = {0,1,2,3,4,5};
//		NRF24L01_RX_AtuoACKPip(i,6,1);	//填充应答信号
	}
}

bool JugeRX()
{
  return (bool)flag_duima;
}

//保存地址到flash
void SaveFlashAddr(u8* buf)
{
  ADDRESS2[0] = buf[0];
  ADDRESS2[1] = buf[1];
  ADDRESS2[2] = buf[2];
  ADDRESS2[3] = buf[3];
  ADDRESS2[4] = buf[4];
  FLASH_Unlock(FLASH_MemType_Data); 
  FLASH_ProgramByte(EEPROM_ADDRESS0,ADDRESS2[0]);
  FLASH_ProgramByte(EEPROM_ADDRESS1,ADDRESS2[1]);
  FLASH_ProgramByte(EEPROM_ADDRESS2,ADDRESS2[2]);
  FLASH_ProgramByte(EEPROM_ADDRESS3,ADDRESS2[3]);
  FLASH_ProgramByte(EEPROM_ADDRESS4,ADDRESS2[4]);
  FLASH_Lock(FLASH_MemType_Data); 
}

void SaveFlashCGAddr(u8* buf)
{
  ADDRESS4[0] = buf[0] + ADDRESS3[0];
  ADDRESS4[1] = buf[1] + ADDRESS3[1];
  ADDRESS4[2] = buf[2] + ADDRESS3[2];
  ADDRESS4[3] = buf[3] + ADDRESS3[3];
  ADDRESS4[4] = buf[4] + ADDRESS3[4];
	FLASH_Unlock(FLASH_MemType_Data); 
  FLASH_ProgramByte(EEPROM_CGADDRESS0,ADDRESS4[0]);
  FLASH_ProgramByte(EEPROM_CGADDRESS1,ADDRESS4[1]);
  FLASH_ProgramByte(EEPROM_CGADDRESS2,ADDRESS4[2]);
  FLASH_ProgramByte(EEPROM_CGADDRESS3,ADDRESS4[3]);
  FLASH_ProgramByte(EEPROM_CGADDRESS4,ADDRESS4[4]);
FLASH_Lock(FLASH_MemType_Data); 
}

// 清除DM
void ClearDM()
{
     // debug("clear DM \r\n");
  FLASH_Unlock(FLASH_MemType_Data);
    FLASH_ProgramByte(EEPROM_ADDRESS0,ADDRESS1[0]);
    FLASH_ProgramByte(EEPROM_ADDRESS1,ADDRESS1[1]);
    FLASH_ProgramByte(EEPROM_ADDRESS2,ADDRESS1[2]);
    FLASH_ProgramByte(EEPROM_ADDRESS3,ADDRESS1[3]);
    FLASH_ProgramByte(EEPROM_ADDRESS4,ADDRESS1[4]);
	
	FLASH_ProgramByte(EEPROM_CGADDRESS0,ADDRESS1[0]);
    FLASH_ProgramByte(EEPROM_CGADDRESS1,ADDRESS1[1]);
    FLASH_ProgramByte(EEPROM_CGADDRESS2,ADDRESS1[2]);
    FLASH_ProgramByte(EEPROM_CGADDRESS3,ADDRESS1[3]);
    FLASH_ProgramByte(EEPROM_CGADDRESS4,ADDRESS1[4]);
	FLASH_Lock(FLASH_MemType_Data); 
    ADDRESS2[0] = ADDRESS1[0];
    ADDRESS2[1] = ADDRESS1[1];
    ADDRESS2[2] = ADDRESS1[2];
    ADDRESS2[3] = ADDRESS1[3];
    ADDRESS2[4] = ADDRESS1[4];
	
	ADDRESS4[0] = ADDRESS1[0];
    ADDRESS4[1] = ADDRESS1[1];
    ADDRESS4[2] = ADDRESS1[2];
    ADDRESS4[3] = ADDRESS1[3];
    ADDRESS4[4] = ADDRESS1[4];
	
    InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP0,RF_CH_HZ,ADDRESS2);	//配置接收模式
	InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP1,RF_CH_HZ,ADDRESS4);	//配置接收模式
}
void NRF_DM()
{
  
  DM_num--;
  if(DM_num&0x01)
    LEN_GREEN_Open();
  else LEN_GREEN_Close();
  if(DM_num == 0)
    flag_duima = 0;
}
bool JugeDM()
{
  return (bool)(flag_duima == 0);
}
//接收模式自动接收完成回调函数
void RXD_CallBack(Nrf24l01_PRXStr* prx) 
{
        prx->rxlen = NRF24L01_GetRXLen();
        NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf + prx->hasrxlen,prx->rxlen);	//读取数据
        prx->hasrxlen += prx->rxlen;		
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// 清除RX_DS中断标志
		debug("-->RX_OK ,pip:%d\r\n",NRD24L01_GetPip(prx->status));
		
}


//成功状态：LED闪烁6次，蜂鸣器同步
void StateSuccess();
//DM模式接收完成回调函数
void DMRXD_CallBack(Nrf24l01_PRXStr* prx)
{
	prx->rxlen = NRF24L01_GetRXLen();	
	NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf,prx->rxlen);	//读取数据   	
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// 清除RX_DS中断标志

	if(prx->rxbuf[5] == CMD_DM && prx->rxbuf[6] == MES_DM) 
	{ 
		SaveFlashAddr(prx->rxbuf);
		flag_duima = 0;
		StateSuccess();
	    debug("\r\n---遥控DM完成---\r\n");
	}else
	 if(prx->rxbuf[5] == 'C' && prx->rxbuf[6] == 'G') 
	{ 
		SaveFlashCGAddr(prx->rxbuf);
		flag_duima = 0;
		StateSuccess();
	    debug("\r\n---传感DM完成---\r\n");
	}
	prx->rxlen = 0;
}


//IRQ 中断
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
		
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) 
	{	
		RXprx.IRQCallBack(&RXprx);
	}
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}
