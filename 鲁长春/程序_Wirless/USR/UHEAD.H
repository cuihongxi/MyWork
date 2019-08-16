/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2017-3-30 09:19:37
*Explain:       通用头文件
*
********************************************************************************/

#ifndef __UHEAD_H
#define __UHEAD_H

#include "stm8l15x.h"
#include "stm8l15x_gpio.h"


#define DEBUG_LEVEL 1		//调试接口

#define SWAPPER_UART 0		// 定义串口发射TX脚 1 -->PA2 ；0 -->PC5

#if 	SWAPPER_UART > 0 
#define	SWAPPER_UART_PA2_PC5()		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);	//端口重映射，去掉注释之后USART1为PA2-TX、PA3-RX；注释之后USART1为TX-PC5、RX-PC6；复位之后USART会自动恢复至PC5、PC6
#else
#define	SWAPPER_UART_PA2_PC5()
#endif


#if  DEBUG_LEVEL > 0
#include "stm8l15x_usart.h"
#include "stdio.h"
#define  debug(...)	printf(__VA_ARGS__);
#define	UART_INIT(baud)	do{\
	SWAPPER_UART_PA2_PC5();	\
	CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);\
	USART_Init(USART1,baud, USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx);\
	debug("start:\n");\
}while(0)
#else
#define	UART_INIT(baud)
#define debug(...)   
#endif


#define delay_ms(x)     LSI_delayms(x)
#define delay_us(x)     LSI_delayus(x)


void LSI_delayus(unsigned int  nCount);         //16M 晶振时  延时 1个微妙
void LSI_delayms(unsigned int  nCount);         //16M 晶振时   延时 1个毫秒
#define	GPIO_SET(pin)	GPIO_SetBits(pin)
#define	GPIO_RESET(pin)	GPIO_ResetBits(pin)


#endif

