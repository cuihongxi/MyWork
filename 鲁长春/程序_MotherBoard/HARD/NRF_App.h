#ifndef     __NRF_App_H
#define     __NRF_App_H

#include "uhead.h"
#include "NRF24L01_AUTO_ACK.H"

extern  Nrf24l01_PRXStr 	prx 		;				// NRF接收结构体
extern  u8 			txbuf[7] 	;				// nrf发送缓存
extern  u8 			rxbuf[7]       ;				// nrf接收缓存


void NRF_Function();/*nrf接收函数*/

#endif
