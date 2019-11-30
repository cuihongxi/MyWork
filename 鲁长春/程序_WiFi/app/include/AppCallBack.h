/**
 * 2019年10月12日06:59:26
 * 应用的回调函数
 */

#ifndef		_AppCallBack_H
#define		_AppCallBack_H

#include "UHEAD.H"
#include "smartconfig.h"		// 智能配网



#define		Sector_STA_INFO		0			// 【STA参数】保存扇区

#define	LED_ON()		MYGPIO_RESET(2)
#define	LED_OFF()		MYGPIO_SET(2)

void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb(void *arg);				// 发送成功回调函数
void ICACHE_FLASH_ATTR ESP8266_TCP_Disconnect_Cb_JX(void *arg);		// TCP连接断开成功的回调函数
void ICACHE_FLASH_ATTR ESP8266_TCP_Break_Cb_JX(void *arg,sint8 err);// TCP连接异常断开时的回调函数
void ESP8266_WIFI_Recv_Cb(void * arg, char * pdata, unsigned short len);// 收到数据的回调函数
void ICACHE_FLASH_ATTR ESP8266_TCP_Connect_Cb_JX(void *arg);		// TCP连接建立成功的回调函数
void ICACHE_FLASH_ATTR DNS_Over_Cb_JX(const char * name, ip_addr_t *ipaddr, void *arg);// DNS_域名解析结束_回调函数
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata);// SmartConfig状态发生改变时，进入此回调函数
void ICACHE_FLASH_ATTR	OS_Timer_CB(void);							// 定时器回调函数
void ICACHE_FLASH_ATTR RxOverCallBack(rxBuffStr* rxstr);			// 串口接收数据回调函数

#endif
