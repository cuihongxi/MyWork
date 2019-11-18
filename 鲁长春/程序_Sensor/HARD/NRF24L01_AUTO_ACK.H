/*
*   NRF24L01的自动应答模式，双向发送接收
*	2019-8-17
*/

#ifndef __NRF24L01_AUTO_ACK_H
#define __NRF24L01_AUTO_ACK_H

#include "UHEAD.H"
#include "24l01.h"


//发射模式结构体
typedef struct PTXStr{
	u8 pip;								// 通道号
	u8 rf_ch;							// 频率
	u8 status;
	u8* txbuf;							// 发射缓存
	u8* rxbuf;							// 接收缓存
	u8 txlen;							// 发送长度
	u8 rxlen;							// 接收长度
	u8 hastxlen;						// 已发送长度
	u8 hasrxlen;						// 已接收的长度
	u8 flag_sendfinish;					// 发送完成标志
	u8 reuse_times;						// 发送不成功时重发的次数
	void(*IRQCallBack)(struct PTXStr*);	// 回调函数，放在IRQ中断中
	void(*RXDCallBack)(struct PTXStr*);	// 接收完成回调函数
	void(*TXDCallBack)(struct PTXStr*);	// 发送完成回调函数
	void(*MAXTXCallBack)(struct PTXStr*);// 达到最大发射次数回调函数
	
}Nrf24l01_PTXStr;

//接收模式结构体
typedef struct PRXStr{
	u8 pip;								// 通道号
	u8 rf_ch;							// 频率
	u8 status;
	u8* txbuf;							// 发射缓存
	u8* rxbuf;							// 接收缓存
	u8 txlen;							// 发送长度
	u8 rxlen;							// 接收长度
	u8 hasrxlen;						// 已接收的长度
	void(*IRQCallBack)(struct PRXStr*);	// 回调函数，放在IRQ中断中
	void(*RXDCallBack)(struct PRXStr*);	// 接收完成回调函数
	void(*TXDCallBack)(struct PRXStr*);	// 发送完成回调函数

}Nrf24l01_PRXStr;

#define		NRF_GPIO_IRQMODE		GPIO_Mode_In_PU_IT	// 上拉带中断

/**
 * IRQ中断功能配置
 */
#define		IRQ_EXTI_SET()	do{\
								disableInterrupts();\
								EXTI_SetPinSensitivity(EXTI_Pin_4,EXTI_Trigger_Falling);\
								enableInterrupts();\
							}while(0)



/*函数*/
/*发射模式*/
void InitNRF_AutoAck_PTX(Nrf24l01_PTXStr* ptx,u8* rxbuf,u8 rxlen,u8 pip,u8 rf_ch);	// 初始化发射模式
void NRF24L01_PTXInMain(Nrf24l01_PTXStr* ptx, u8* txbuf,u8 txlen); 			// 主函数中的发送函数
void NRF24L01_PTXInMainReset(Nrf24l01_PTXStr* ptx);					// 复位主函数的发送
void NRF_AutoAck_TxPacket(Nrf24l01_PTXStr* ptx, u8 *txbuf,u8 size);			// 发射模式，发送短消息

void Default_IRQCallBack_PTX(Nrf24l01_PTXStr* ptx);	// 发射中断回调函数
void MAXTX_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// 达到最大发射次数默认回调函数
void TXD_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// 发射模式自动发射完成回调函数
void RXD_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// 发射模式自动接收完成回调函数

/*接收模式*/
void InitNRF_AutoAck_PRX(Nrf24l01_PRXStr* prx,u8* rxbuf,u8* txbuf,u8 txlen,u8 pip,u8 rf_ch);	// 初始化接收模式

void Default_IRQCallBack_PRX(Nrf24l01_PRXStr* prx); // 默认的接收回调函数
void RXD_CallBack_PRX(Nrf24l01_PRXStr* prx);		// 接收模式自动接收完成回调函数
void TXD_CallBack_PRX(Nrf24l01_PRXStr* prx); 		// 接收模式自动发射完成回调函数
void RXD_ACK_CallBack(Nrf24l01_PRXStr* prx);		// 应答信号回调


#endif













