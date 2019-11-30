/**
 * 2019��10��12��06:59:26
 * Ӧ�õĻص�����
 */

#ifndef		_AppCallBack_H
#define		_AppCallBack_H

#include "UHEAD.H"
#include "smartconfig.h"		// ��������



#define		Sector_STA_INFO		0			// ��STA��������������

#define	LED_ON()		MYGPIO_RESET(2)
#define	LED_OFF()		MYGPIO_SET(2)

void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb(void *arg);				// ���ͳɹ��ص�����
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb_JX(void *arg);		// TCP���ӶϿ��ɹ��Ļص�����
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err);// TCP�����쳣�Ͽ�ʱ�Ļص�����
void ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len);// �յ����ݵĻص�����
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb_JX(void *arg);		// TCP���ӽ����ɹ��Ļص�����
void ICACHE_FLASH_ATTR DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg);// DNS_������������_�ص�����
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata);// SmartConfig״̬�����ı�ʱ������˻ص�����
void ICACHE_FLASH_ATTR	OS_Timer_CB(void);							// ��ʱ���ص�����
void ICACHE_FLASH_ATTR RxOverCallBack(rxBuffStr* rxstr);			// ���ڽ������ݻص�����

#endif
