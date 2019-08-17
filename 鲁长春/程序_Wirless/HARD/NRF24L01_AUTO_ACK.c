
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
	
}

//�������еķ��ͺ���
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
	}

}

//�ﵽ��������Ĭ�ϻص�����
void MAXTX_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug("ERROR! MAX_TX!! ");			
	NRF24L01_Write_Reg(FLUSH_TX,0x00); //���tx fifo�Ĵ���	//********��Ҫ*********
}

//����ģʽ�Զ�������ɻص�����
void RXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	  ptx->rxlen = NRF24L01_GetRXLen();
	  debug(" RX_OK ");
	  NRF24L01_Read_Buf(RD_RX_PLOAD,ptx->rxbuf,ptx->rxlen);//��ȡ����
	  for(u8 i=0;i<ptx->rxlen;i++)
	  {
		debug(" ptx[%d]: %c ",i,ptx->rxbuf[i]);
	  }
	  if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_RX_FULL))
			NRF24L01_Write_Reg(FLUSH_RX,0x00);//���RX FIFO�Ĵ��� 
}

//����ģʽ�Զ�������ɻص�����
void TXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug(" TX_OK  ");
	if(ptx->hastxlen < ptx->txlen)
	{
		if((ptx->txlen - ptx->hastxlen) > 32 ) {
			NRF24L01_TxPacket(ptx->txbuf + ptx->hastxlen,32)	;
			ptx->hastxlen += 32;
		}else 		
		{
			NRF24L01_TxPacket(ptx->txbuf,ptx->txlen);
			ptx->hastxlen += ptx->txlen;
		}
	}else
	{
		ptx->flag_sendfinish = TRUE;
	}
}

//�жϻص�����
void Default_IRQCallBack_PTX(Nrf24l01_PTXStr* ptx)
{
  	if(READ_IRQ_IN == 0)
	{
		ptx->status = NRF24L01_Read_Reg(STATUS);  			 	//��ȡ״̬�Ĵ�����ֵ
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,ptx->status); 	//���TX_DS��MAX_RT�жϱ�־ 
		if(ptx->status&TX_OK)//�������
		{
			ptx->TXDCallBack(ptx);
		}else
		 if(ptx->status & MAX_TX)//�ﵽ����ط�����
		{
		  	ptx->MAXTXCallBack(ptx);
		}
		if(ptx->status & RX_OK)				//���շ�����Ϣ����
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

/*********************************************************************************************************/

//����ģʽ�Զ�������ɻص�����
void RXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
		prx->rxlen = NRF24L01_GetRXLen();
		NRF24L01_RX_AtuoACKPip(prx->txbuf,prx->txlen,prx->pip);	//���Ӧ���ź�
		debug("len = %d  \r\n",prx->rxlen);
		NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf,prx->rxlen);	//��ȡ����				 
		 if(NRF24L01_Read_Reg(NRF_FIFO_STATUS) &(1<<FIFO_RX_FULL))
				NRF24L01_Write_Reg(FLUSH_RX,0x00);				// ���RX FIFO�Ĵ��� 
		for(u8 i=0;i<prx->rxlen;i++)
		  {
		  	debug(" prx[%d]: %c ",i,prx->rxbuf[i]);
		  }
}

//����ģʽ�Զ�������ɻص�����
void TXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
	debug(" TX_OK  ");
}
//Ĭ�ϵĽ��ջص�����
void Default_IRQCallBack_PRX(Nrf24l01_PRXStr* prx)
{
	 if(READ_IRQ_IN == 0)
	{	    							      
		prx->status = NRF24L01_Read_Reg(STATUS);  				//��ȡ״̬�Ĵ�����ֵ      
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,prx->status); 	//���TX_DS��MAX_RT�жϱ�־
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
	Init_NRF24L01(prx->pip,prx->rf_ch);
	NRF24L01_GPIO_IRQ();
	prx->IRQCallBack = Default_IRQCallBack_PRX;
	prx->RXDCallBack = RXD_CallBack_PRX;
	prx->TXDCallBack = TXD_CallBack_PRX;
	NRF24L01_RX_Mode();                         //���ý���ģʽ 
}

 