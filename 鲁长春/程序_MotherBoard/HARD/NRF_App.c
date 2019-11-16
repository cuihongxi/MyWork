#include "NRF_App.h"
#include "NRF24L01_AUTO_ACK.H"
#include "24l01.h"
#include "CUI_RTOS.H"
#include "LED_SHOW.H"

Nrf24l01_PRXStr 	RXprx 		= {0};				// NRFæŽ¥æ”¶ç»“æž„ä½“
Nrf24l01_PTXStr 	TXptx 		= {0};				// NRFå‘é€ç»“æž„ä½“
u8 			RXtxbuf[7] 	= {0,0,0,0,0,'O','K'};		// nrfå‘é€ç¼“å­˜
u8 			RXrxbuf[7] 	= {0};				        // nrfæŽ¥æ”¶ç¼“å­˜
u8 			TXtxbuf[7] 	= {0};		
u8 			TXrxbuf[7] 	= {0};				        
u8          DM_num = 0;

extern  u8 		            flag_duima  		;	//å¯¹ç çŠ¶æ€
extern  u8 		            flag_duima_clear  	;	//æ¸…é™¤å¯¹ç 
extern  TaskStr* 	        taskNRF	            ;
extern  TaskLinkStr* 		tasklink            ;
extern  JugeCStr 		    NRFpowon 	        ;
extern  JugeCStr 		    NRFpowoff 	        ;

void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes);
void NRFReceived();
bool JugeRX();
void NRF_DM();
bool JugeDM();
void RXD_CallBack(Nrf24l01_PRXStr* prx) ;   //æŽ¥æ”¶æ¨¡å¼è‡ªåŠ¨æŽ¥æ”¶å®Œæˆå›žè°ƒå‡½æ•°
void ptxRXD_CallBack(Nrf24l01_PTXStr* ptx);//å‘å°„æ¨¡å¼è‡ªåŠ¨æŽ¥æ”¶å®Œæˆå›žè°ƒå‡½æ•°
/*nrf´¦Àíº¯Êý*/
void NRF_Function()
{   
        if(taskNRF->state == Stop)
        {
          if(flag_duima == 0)
          {
            
            address = ADDRESS2;
            InitNRF_AutoAck_PRX(&RXprx,RXrxbuf,RXtxbuf,sizeof(RXtxbuf),BIT_PIP0,RF_CH_HZ);	//ÅäÖÃ½ÓÊÕÄ£Ê½
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
              address = ADDRESS1;
              InitNRF_AutoAck_PTX(&TXptx,TXrxbuf,sizeof(TXrxbuf),BIT_PIP0,RF_CH_HZ);	//·¢ËÍÄ£Ê½
              TXptx.RXDCallBack = ptxRXD_CallBack;
              TXptx.txbuf = TXtxbuf;
			  NRF24L01_GPIO_Lowpower();
              DM_num = DM_NUM;
              OS_AddJudegeFunction(taskNRF,NRF_DM,800,JugeDM);
          }
           OS_AddTask(tasklink,taskNRF);	
        }
}

void NRF_SendCMD(Nrf24l01_PTXStr* ptx,u8* addr,u8 cmd , u8 mes)
{

    NRF24L01_PWR(1);
    ptx->txbuf[0] = addr[0];
    ptx->txbuf[1] = addr[1];
    ptx->txbuf[2] = addr[2];
    ptx->txbuf[3] = addr[3];
    ptx->txbuf[4] = addr[4];
    ptx->txbuf[5] = cmd;
    ptx->txbuf[6] = mes;
    
  NRF_AutoAck_TxPacket(ptx,ptx->txbuf,7);
  
}

void ChangeNRFCmd(u8* buf);
// Ö÷Ñ­»·ÖÐÔËÐÐµÄ£¬ÏÔÊ¾½ÓÊÕµ½µÄÊý¾Ý
void NRFReceived()
{
	if(RXprx.hasrxlen)
	{
		debug("hasrxlen = %d :\r\n",RXprx.hasrxlen);		
		for(u8 i=0;i<RXprx.hasrxlen;i++)
		  {
			debug("rxbuf[%d]=%d	",i,RXprx.rxbuf[i]);
		  }		
		debug("\r\n------\r\n");
                //¸ù¾ÝÊÕµ½µÄ×Ö½Ú£¬Ó³Éä³öÏàÓ¦µÄ¿ØÖÆÃüÁî
                ChangeNRFCmd(RXprx.rxbuf);
		RXprx.hasrxlen = 0;
	}
}

bool JugeRX()
{
  return (bool)flag_duima;
}

void NRF_DM()
{
  
  DM_num--;
  if(DM_num&0x01)
    LEN_GREEN_Open();
  else LEN_GREEN_Close();
  if(DM_num>0)
    NRF_SendCMD(&TXptx,ADDRESS2,CMD_DM,MES_DM);
  else flag_duima = 0;
}
bool JugeDM()
{
  return (bool)(flag_duima == 0);
}
//½ÓÊÕÄ£Ê½×Ô¶¯½ÓÊÕÍê³É»Øµ÷º¯Êý
void RXD_CallBack(Nrf24l01_PRXStr* prx) 
{
        prx->txbuf[0] = prx->rxbuf[0];
        prx->txbuf[1] = prx->rxbuf[1];
        prx->txbuf[2] = prx->rxbuf[2];
        prx->txbuf[3] = prx->rxbuf[3];
        prx->txbuf[4] = prx->rxbuf[4];
        prx->txbuf[5] = 'O';
        prx->txbuf[6] = 'K';
        NRF24L01_RX_AtuoACKPip(prx->txbuf,prx->txlen,prx->pip);//Ìî³äÓ¦´ðÐÅºÅ	
        prx->rxlen = NRF24L01_GetRXLen();
        NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf + prx->hasrxlen,prx->rxlen);	//¶ÁÈ¡Êý¾Ý
        prx->hasrxlen += prx->rxlen;		
        NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// Çå³ýRX_DSÖÐ¶Ï±êÖ¾
        debug("RX_OK ");
}

bool ptxJugeDMok(Nrf24l01_PTXStr* ptx)
{
//  for(u8 i=0;i < ptx->rxlen; i++)
//  {  
//    if(ptx->rxbuf[i] ^ ptx->txbuf[i]) return (bool)0;
//  }
  if(ptx->rxbuf[5] ^ 'D') return (bool)0;
  if(ptx->rxbuf[6] ^ 'M') return (bool)0;
  return (bool)1;
}
//·¢ÉúÄ£Ê½½ÓÊÕÍê³É»Øµ÷º¯Êý
void ptxRXD_CallBack(Nrf24l01_PTXStr* ptx)
{
        ptx->rxlen = NRF24L01_GetRXLen();
	NRF24L01_Read_Buf(RD_RX_PLOAD,ptx->rxbuf,ptx->rxlen);	//¶Á½ÓÊÕÊý¾Ý
        if(ptxJugeDMok(ptx))  {debug("Åä¶Ô³É¹¦\n");LEN_GREEN_Close();flag_duima = 0;} 
   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// Çå±êÖ¾Î»
}


//IRQ ÖÐ¶Ï
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
		
	if(GPIO_READ(NRF24L01_IRQ_PIN) == 0) 
	{	
      if(flag_duima == 0)	
		RXprx.IRQCallBack(&RXprx);
      else
        TXptx.IRQCallBack(&TXptx);
	}
   	EXTI_ClearITPendingBit (EXTI_IT_Pin2);
}
