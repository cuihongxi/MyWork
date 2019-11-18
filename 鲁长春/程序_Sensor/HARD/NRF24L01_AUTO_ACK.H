/*
*   NRF24L01���Զ�Ӧ��ģʽ��˫���ͽ���
*	2019-8-17
*/

#ifndef __NRF24L01_AUTO_ACK_H
#define __NRF24L01_AUTO_ACK_H

#include "UHEAD.H"
#include "24l01.h"


//����ģʽ�ṹ��
typedef struct PTXStr{
	u8 pip;								// ͨ����
	u8 rf_ch;							// Ƶ��
	u8 status;
	u8* txbuf;							// ���仺��
	u8* rxbuf;							// ���ջ���
	u8 txlen;							// ���ͳ���
	u8 rxlen;							// ���ճ���
	u8 hastxlen;						// �ѷ��ͳ���
	u8 hasrxlen;						// �ѽ��յĳ���
	u8 flag_sendfinish;					// ������ɱ�־
	u8 reuse_times;						// ���Ͳ��ɹ�ʱ�ط��Ĵ���
	void(*IRQCallBack)(struct PTXStr*);	// �ص�����������IRQ�ж���
	void(*RXDCallBack)(struct PTXStr*);	// ������ɻص�����
	void(*TXDCallBack)(struct PTXStr*);	// ������ɻص�����
	void(*MAXTXCallBack)(struct PTXStr*);// �ﵽ���������ص�����
	
}Nrf24l01_PTXStr;

//����ģʽ�ṹ��
typedef struct PRXStr{
	u8 pip;								// ͨ����
	u8 rf_ch;							// Ƶ��
	u8 status;
	u8* txbuf;							// ���仺��
	u8* rxbuf;							// ���ջ���
	u8 txlen;							// ���ͳ���
	u8 rxlen;							// ���ճ���
	u8 hasrxlen;						// �ѽ��յĳ���
	void(*IRQCallBack)(struct PRXStr*);	// �ص�����������IRQ�ж���
	void(*RXDCallBack)(struct PRXStr*);	// ������ɻص�����
	void(*TXDCallBack)(struct PRXStr*);	// ������ɻص�����

}Nrf24l01_PRXStr;

#define		NRF_GPIO_IRQMODE		GPIO_Mode_In_PU_IT	// �������ж�

/**
 * IRQ�жϹ�������
 */
#define		IRQ_EXTI_SET()	do{\
								disableInterrupts();\
								EXTI_SetPinSensitivity(EXTI_Pin_4,EXTI_Trigger_Falling);\
								enableInterrupts();\
							}while(0)



/*����*/
/*����ģʽ*/
void InitNRF_AutoAck_PTX(Nrf24l01_PTXStr* ptx,u8* rxbuf,u8 rxlen,u8 pip,u8 rf_ch);	// ��ʼ������ģʽ
void NRF24L01_PTXInMain(Nrf24l01_PTXStr* ptx, u8* txbuf,u8 txlen); 			// �������еķ��ͺ���
void NRF24L01_PTXInMainReset(Nrf24l01_PTXStr* ptx);					// ��λ�������ķ���
void NRF_AutoAck_TxPacket(Nrf24l01_PTXStr* ptx, u8 *txbuf,u8 size);			// ����ģʽ�����Ͷ���Ϣ

void Default_IRQCallBack_PTX(Nrf24l01_PTXStr* ptx);	// �����жϻص�����
void MAXTX_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// �ﵽ��������Ĭ�ϻص�����
void TXD_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// ����ģʽ�Զ�������ɻص�����
void RXD_CallBack_PTX(Nrf24l01_PTXStr* ptx);	// ����ģʽ�Զ�������ɻص�����

/*����ģʽ*/
void InitNRF_AutoAck_PRX(Nrf24l01_PRXStr* prx,u8* rxbuf,u8* txbuf,u8 txlen,u8 pip,u8 rf_ch);	// ��ʼ������ģʽ

void Default_IRQCallBack_PRX(Nrf24l01_PRXStr* prx); // Ĭ�ϵĽ��ջص�����
void RXD_CallBack_PRX(Nrf24l01_PRXStr* prx);		// ����ģʽ�Զ�������ɻص�����
void TXD_CallBack_PRX(Nrf24l01_PRXStr* prx); 		// ����ģʽ�Զ�������ɻص�����
void RXD_ACK_CallBack(Nrf24l01_PRXStr* prx);		// Ӧ���źŻص�


#endif













