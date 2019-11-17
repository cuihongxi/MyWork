
#include "NRF24L01_AUTO_ACK.H"
#include "stm8l15x_exti.h"


//��ʼ��24L01��IRQ IO��
void NRF24L01_GPIO_IRQ(void)
{
  GPIO_Init(NRF24L01_IRQ_PIN,NRF_GPIO_IRQMODE);      				//��IRQΪ�͵�ƽʱΪ�жϴ���
  IRQ_EXTI_SET();
}

//��λ�������ķ���
void NRF24L01_PTXInMainReset(Nrf24l01_PTXStr* ptx)
{
	ptx->flag_sendfinish = FALSE;
	ptx->hastxlen = 0;
	ptx->hasrxlen = 0;
}
//�������еķ��ͺ���,ֻ��һ�Σ��������32���������ж��з���
void NRF24L01_PTXInMain(Nrf24l01_PTXStr* ptx, u8* txbuf,u8 txlen)
{
	if (ptx->flag_sendfinish == FALSE)
	{
		if(ptx->hastxlen == 0)
		{
			ptx->txbuf = txbuf;
			ptx->txlen = txlen;
			if(ptx->txlen > 32 )
			{
				NRF24L01_TxPacket(ptx->txbuf,32)	;
				ptx->hastxlen += 32;
			}else 		
			{
				NRF24L01_TxPacket(ptx->txbuf,ptx->txlen);
				ptx->hastxlen += ptx->txlen;
			}
		}		
	}else
	{
	//  	ptx->txbuf[0] ++;
	//	NRF24L01_PTXInMainReset(ptx);
		
	}
	  

}

void NRF24L01_RESUSE();
//�ﵽ��������Ĭ�ϻص�����
void MAXTX_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug("ERROR! MAX_TX!! ");

	//else
	//{
		NRF24L01_Write_Reg(FLUSH_TX,0x00); //���tx fifo�Ĵ���	//********��Ҫ*********
		NRF24L01_PTXInMainReset(ptx);

		//NRF24L01_PWR(0);	
	//}
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RPTX)); 	// ���R�жϱ�־
	if(ptx->reuse_times)
	{
		ptx->reuse_times --;
		//NRF24L01_RESUSE_TX();
		NRF24L01_RESUSE();
	}
}

//����ģʽ�Զ�������ɻص�����
void RXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	    debug(" RX_OK ");	
		ptx->rxlen = NRF24L01_GetRXLen();
		NRF24L01_Read_Buf(RD_RX_PLOAD,ptx->rxbuf + ptx->hasrxlen,ptx->rxlen);	//��ȡ����
		ptx->hasrxlen += ptx->rxlen;
	//	debug("rxlen = %d :\r\n",ptx->rxlen);		
//		for(u8 i=0;i<ptx->rxlen;i++)
//			  {
//				debug(" %d ",ptx->rxbuf[i]);
//			  }   
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// ���RX_DS�жϱ�־
		NRF24L01_PWR(0);
}

//����ģʽ�Զ�������ɻص�����
void TXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug(" TX_OK  ");
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1<<STATUS_BIT_IRT_TXD)); 	// ���TX_DS�жϱ�־
	if(ptx->hastxlen < ptx->txlen)
	{
		if((ptx->txlen - ptx->hastxlen) > 32 ) {
			NRF24L01_TxPacket(ptx->txbuf + ptx->hastxlen,32)	;
			ptx->hastxlen += 32;
		}else 		
		{
			NRF24L01_TxPacket(ptx->txbuf + ptx->hastxlen,(ptx->txlen - ptx->hastxlen));
			ptx->hastxlen = ptx->txlen ;
		}
	}else
	{
		ptx->flag_sendfinish = TRUE;
		//debug("send OK");
	}
}

//�����жϻص�����
void Default_IRQCallBack_PTX(Nrf24l01_PTXStr* ptx)
{
  	if(READ_IRQ_IN == 0)
	{
		ptx->status = NRF24L01_GetStatus();  			 		// ��ȡ״̬�Ĵ�����ֵ
		if(ptx->status&TX_OK)									// �������
		{
			ptx->TXDCallBack(ptx);
		}else
		 if(ptx->status & MAX_TX)								// �ﵽ����ط�����
		{
		  	ptx->MAXTXCallBack(ptx);
		}
		if(ptx->status & RX_OK)									// ���շ�����Ϣ����
		{
		  ptx->RXDCallBack(ptx);
		}

		debug("\r\n");
	}
}

//��ʼ������ģʽ
void InitNRF_AutoAck_PTX(Nrf24l01_PTXStr* ptx,u8* rxbuf,u8 rxlen,u8 pip,u8 rf_ch)
{
	ptx->rxbuf = rxbuf;
	ptx->rxlen = rxlen;
	ptx->flag_sendfinish  = FALSE;
	ptx->hastxlen = 0;
	ptx->hasrxlen = 0;
	
	ptx->pip = pip;
	ptx->rf_ch = rf_ch;
	
	Init_NRF24L01(ptx->pip,ptx->rf_ch);
	NRF24L01_GPIO_IRQ();
	ptx->IRQCallBack = Default_IRQCallBack_PTX;
	ptx->RXDCallBack = RXD_CallBack_PTX;
	ptx->TXDCallBack = TXD_CallBack_PTX;
	ptx->MAXTXCallBack = MAXTX_CallBack_PTX;
	NRF24L01_TX_Mode();				// ����Ϊ����ģʽ


}

//����ģʽ�����Ͷ���Ϣ
void NRF_AutoAck_TxPacket(Nrf24l01_PTXStr* ptx, u8 *txbuf,u8 size)
{
	ptx->txbuf = txbuf;
	ptx->txlen = size;
	ptx->hastxlen = 0;
	ptx->hasrxlen = 0;
	ptx->flag_sendfinish  = FALSE;
	ptx->reuse_times = REUSE_TIMES;
	NRF24L01_TxPacket(ptx->txbuf,ptx->txlen);
	ptx->hastxlen += ptx->txlen;	
}

/******************************************����ģʽ***************************************************************/


void RXD_ACK_CallBack(Nrf24l01_PRXStr* prx)
{
	prx->txbuf[0] ++;
}
//����ģʽ�Զ�������ɻص�����
void RXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
  		RXD_ACK_CallBack(prx);
  		NRF24L01_RX_AtuoACKPip(prx->txbuf,prx->txlen,prx->pip);	//���Ӧ���ź�		
		prx->rxlen = NRF24L01_GetRXLen();
		NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf + prx->hasrxlen,prx->rxlen);	//��ȡ����
		prx->hasrxlen += prx->rxlen;		
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// ���RX_DS�жϱ�־
		debug("RX_OK ");
}

//����ģʽ�Զ�������ɻص�����
void TXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
	debug(" TX_OK  ");
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_TXD)); 	// ���TX_DS�жϱ�־
}
//Ĭ�ϵĽ��ջص�����
void Default_IRQCallBack_PRX(Nrf24l01_PRXStr* prx)
{
	 if(READ_IRQ_IN == 0)
	{	    							      
		prx->status = NRF24L01_GetStatus();  					// ��ȡ״̬�Ĵ�����ֵ      
		//NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,prx->status); 	// ���TX_DS��MAX_RT�жϱ�־
		if(prx->status & RX_OK)//���յ�����
		{
		  prx->RXDCallBack(prx);
		}
		if(prx->status & TX_OK)
		{
		  prx->TXDCallBack(prx);	
		}				
		debug("\r\n");
	}
}


//��ʼ������ģʽ
void InitNRF_AutoAck_PRX(Nrf24l01_PRXStr* prx,u8* rxbuf,u8* txbuf,u8 txlen,u8 pip,u8 rf_ch)
{
	prx->rxbuf = rxbuf;
	prx->pip = pip;
	prx->rf_ch = rf_ch;
	prx->txbuf = txbuf;
	prx->txlen = txlen;
	prx->hasrxlen = 0;
	Init_NRF24L01(prx->pip,prx->rf_ch);
	NRF24L01_GPIO_IRQ();
	prx->IRQCallBack = Default_IRQCallBack_PRX;
	prx->RXDCallBack = RXD_CallBack_PRX;
	prx->TXDCallBack = TXD_CallBack_PRX;
	NRF24L01_RX_Mode();                         //���ý���ģʽ 
}

 