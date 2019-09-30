/********************************************************************************
*Author :       explorersoftware.taobao.com 
*Time   :       2019��8��24��15:44:36
*Explain:       ͨ��ͷ�ļ�
*
********************************************************************************/

#ifndef __UHEAD_H
#define __UHEAD_H

#include "stm8l15x.h"
#include "stm8l15x_gpio.h"
#include "UDATA.H"



#define DEBUG_LEVEL 1		//���Խӿ�

#define SWAPPER_UART 0		// ���崮�ڷ���TX�� 1 -->PA2 ��0 -->PC3

#if 	SWAPPER_UART > 0 
#define	SWAPPER_UART_PA2_PC3()		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);	//�˿���ӳ�䣬ȥ��ע��֮��USART1ΪPA2-TX��PA3-RX��ע��֮��USART1ΪTX-PC5��RX-PC6����λ֮��USART���Զ��ָ���PC5��PC6
#else
#define	SWAPPER_UART_PA2_PC3()
#endif


#if  DEBUG_LEVEL > 0
#include "stm8l15x_usart.h"
#include "stdio.h"
#define  debug(...) 		printf(__VA_ARGS__);
#define	UART_INIT(baud)	do{\
	RST->CR = 0xD0;\
	SWAPPER_UART_PA2_PC3();	\
	CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);\
	USART_Init(USART1,baud, USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx);\
	debug("\r\nstart:\r\n");\
}while(0)
#else
#define	UART_INIT(baud) RST->CR = 0x00;	//�ָ�RST��Ϊ��λ��
#define debug(...)   
#endif

#define BEEP_SW 0
#if BEEP_SW > 0
#define		GPIO_BEEP			GPIOA,GPIO_Pin_0		// ������
#else
#define		GPIO_BEEP			GPIOC,GPIO_Pin_0		// ������
#endif

#ifndef 	TRUE
#define		TRUE	1
#endif

#ifndef 	FALSE
#define		FALSE	0
#endif


#define delay_ms(x)     	LSI_delayms(x/8)
#define delay_us(x)     	LSI_delayus(x/8)

#define	GPIO_SET(pin)		GPIO_SetBits(pin)
#define	GPIO_RESET(pin)		GPIO_ResetBits(pin)
#define	GPIO_READ(pin)		GPIO_ReadInputDataBit(pin)



//��ʱ�ж���������
typedef struct{
	u8 start;			//������ʱ
	u8 switchon;		//��ʱ�������ش�
	u32 counter;		//����
}JugeCStr;

typedef enum {ISOK = 0, ISERROR = 1} Flag_Status;

/*���ú���*/

void LSI_delayus(unsigned int  nCount);   //16M ����ʱ  ��ʱ 1��΢��
void LSI_delayms(unsigned int  nCount);
bool Juge_counter(JugeCStr* juge, u32 swdat);

//���ϱ���
#define		ERROR_OK		0	// �޹���
#define		ERROR_BAT		(0x01)	// BAT��ѹ����
#define		ERROR_MOTOR		(0x02)	// ������ת����ʱ
#define		ERROR_BH		(0x10)	// BHû�в���


#endif

//	debug("sys clk souce: %d\r\n frq: %lu\r\n",CLK_GetSYSCLKSource(),CLK_GetClockFreq());