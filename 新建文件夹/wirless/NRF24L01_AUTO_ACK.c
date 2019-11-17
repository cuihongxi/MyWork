
#include "NRF24L01_AUTO_ACK.H"
#include "stm8l15x_exti.h"


//初始化24L01的IRQ IO口
void NRF24L01_GPIO_IRQ(void)
{
  GPIO_Init(NRF24L01_IRQ_PIN,NRF_GPIO_IRQMODE);      				//当IRQ为低电平时为中断触发
  IRQ_EXTI_SET();
}

//复位主函数的发送
void NRF24L01_PTXInMainReset(Nrf24l01_PTXStr* ptx)
{
	ptx->flag_sendfinish = FALSE;
	ptx->hastxlen = 0;
	ptx->hasrxlen = 0;
}
//主函数中的发送函数,只发一次，其余大于32的数据在中断中发送
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
//达到最大发射次数默认回调函数
void MAXTX_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug("ERROR! MAX_TX!! ");

	//else
	//{
		NRF24L01_Write_Reg(FLUSH_TX,0x00); //清除tx fifo寄存器	//********重要*********
		NRF24L01_PTXInMainReset(ptx);

		//NRF24L01_PWR(0);	
	//}
	
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RPTX)); 	// 清除R中断标志
	if(ptx->reuse_times)
	{
		ptx->reuse_times --;
		//NRF24L01_RESUSE_TX();
		NRF24L01_RESUSE();
	}
}

//发射模式自动接收完成回调函数
void RXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	    debug(" RX_OK ");	
		ptx->rxlen = NRF24L01_GetRXLen();
		NRF24L01_Read_Buf(RD_RX_PLOAD,ptx->rxbuf + ptx->hasrxlen,ptx->rxlen);	//读取数据
		ptx->hasrxlen += ptx->rxlen;
	//	debug("rxlen = %d :\r\n",ptx->rxlen);		
//		for(u8 i=0;i<ptx->rxlen;i++)
//			  {
//				debug(" %d ",ptx->rxbuf[i]);
//			  }   
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// 清除RX_DS中断标志
		NRF24L01_PWR(0);
}

//发射模式自动发射完成回调函数
void TXD_CallBack_PTX(Nrf24l01_PTXStr* ptx)
{
	debug(" TX_OK  ");
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1<<STATUS_BIT_IRT_TXD)); 	// 清除TX_DS中断标志
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

//发射中断回调函数
void Default_IRQCallBack_PTX(Nrf24l01_PTXStr* ptx)
{
  	if(READ_IRQ_IN == 0)
	{
		ptx->status = NRF24L01_GetStatus();  			 		// 读取状态寄存器的值
		if(ptx->status&TX_OK)									// 发送完成
		{
			ptx->TXDCallBack(ptx);
		}else
		 if(ptx->status & MAX_TX)								// 达到最大重发次数
		{
		  	ptx->MAXTXCallBack(ptx);
		}
		if(ptx->status & RX_OK)									// 接收方有信息返回
		{
		  ptx->RXDCallBack(ptx);
		}

		debug("\r\n");
	}
}

//初始化发射模式
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
	NRF24L01_TX_Mode();				// 配置为发送模式


}

//发射模式，发送短消息
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

/******************************************接收模式***************************************************************/


void RXD_ACK_CallBack(Nrf24l01_PRXStr* prx)
{
	prx->txbuf[0] ++;
}
//接收模式自动接收完成回调函数
void RXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
  		RXD_ACK_CallBack(prx);
  		NRF24L01_RX_AtuoACKPip(prx->txbuf,prx->txlen,prx->pip);	//填充应答信号		
		prx->rxlen = NRF24L01_GetRXLen();
		NRF24L01_Read_Buf(RD_RX_PLOAD,prx->rxbuf + prx->hasrxlen,prx->rxlen);	//读取数据
		prx->hasrxlen += prx->rxlen;		
		NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_RXD)); 	// 清除RX_DS中断标志
		debug("RX_OK ");
}

//接收模式自动发射完成回调函数
void TXD_CallBack_PRX(Nrf24l01_PRXStr* prx) 
{
	debug(" TX_OK  ");
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,(1 << STATUS_BIT_IRT_TXD)); 	// 清除TX_DS中断标志
}
//默认的接收回调函数
void Default_IRQCallBack_PRX(Nrf24l01_PRXStr* prx)
{
	 if(READ_IRQ_IN == 0)
	{	    							      
		prx->status = NRF24L01_GetStatus();  					// 读取状态寄存器的值      
		//NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,prx->status); 	// 清除TX_DS或MAX_RT中断标志
		if(prx->status & RX_OK)//接收到数据
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


//初始化接收模式
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
	NRF24L01_RX_Mode();                         //配置接收模式 
}

 