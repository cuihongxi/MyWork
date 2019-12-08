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
#include "UDATA.H"

#define DEBUG_LEVEL 0		//调试接口

#define SWAPPER_UART 0		// 定义串口发射TX脚 1 -->PA2 ；0 -->PC5

#if 	SWAPPER_UART > 0 
#define	SWAPPER_UART_PA2_PC5()		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);	//端口重映射，去掉注释之后USART1为PA2-TX、PA3-RX；注释之后USART1为TX-PC5、RX-PC6；复位之后USART会自动恢复至PC5、PC6
#else
#define	SWAPPER_UART_PA2_PC5()
#endif


#if  DEBUG_LEVEL > 0
#include "stm8l15x_usart.h"
#include "stdio.h"
#define  debug(...)	USART1_SendSTR(__VA_ARGS__)     //printf(__VA_ARGS__);
#define	UART_INIT(baud)	do{\
	SWAPPER_UART_PA2_PC5();	\
	CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);\
	USART_Init(USART1,baud, USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx);\
	debug("\r\nstart:\r\n");\
}while(0)
#else
#define	UART_INIT(baud)
#define debug(...)   
#endif

#define         IRQ_PERIOD      500  
#define delay_ms(x)     LSI_delayms(x/8)
#define delay_us(x)     LSI_delayus(x/8)
//定时判断数据类型
typedef struct{
	u8 start;			//启动定时
	u8 switchon;		//定时到，开关打开
	u32 counter;		//计数
}JugeCStr;

bool Juge_counter(JugeCStr* juge, u32 swdat);

//nrf 命令 
#define		CMD_DM		0X5A
#define		CMD_AM		0X3A
#define		CMD_Y30		0X2A
#define		CMD_Z		0X1A
#define		CMD_Y		0X0A
#define		CMD_I30		0X6A
#define		CMD_WAKE	0X4A

#define		MES_AM		0X01
#define		MES_AM_CHECK	0X02
#define		MES_Y30_3_1	0X03
#define		MES_Y30_3_2	0X04
#define		MES_Y30_3_3	0X05
#define		MES_Y30_CLEAR	0X0E
#define		MES_Y30_CHECK	0X12

#define		MES_Z_3_1	0X06
#define		MES_Z_3_2	0X07
#define		MES_Z_3_3	0X08
#define		MES_Z		0X0F
#define		MES_Y_3_1	0X09
#define		MES_Y_3_2	0X0A
#define		MES_Y_3_3	0X0B
#define		MES_Y		0X10
#define		MES_DM	    0X0C
#define		MES_CLEARDM	0X0D
#define		MES_I30_ALARM		0X11
#define		MES_WAKE_UP		0X13
#define		MES_WAKE_SLEEP	0X14


void LSI_delayus(unsigned int  nCount);         //16M 晶振时  延时 1个微妙
void LSI_delayms(unsigned int  nCount);         //16M 晶振时   延时 1个毫秒

#define	GPIO_SET(pin)		GPIO_SetBits(pin)
#define	GPIO_RESET(pin)		GPIO_ResetBits(pin)
#define	GPIO_READ(pin)		GPIO_ReadInputDataBit(pin)

#endif

