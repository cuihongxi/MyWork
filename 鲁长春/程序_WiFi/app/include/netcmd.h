/**
 * 关于字符串网络命令的处理
 * 2019年10月13日07:50:16
 */

#ifndef		__netcmd_h
#define		__netcmd_h

#include "uhead.h"

#define		TCP_CONNECT			"tcpcon:"		// TCP_CLIENT模式连接服务器的起始头  tcp_c:192.168.31.67:6666
#define		MESSAGE				"mes:"			// 信息开始的起始头 mes:ni hao a
#define		DIS_CONNECT			"disconnect"	// 断开与TCP_SERVER的连接
#define		NET_CONNECT			"net:"			// 通过域名访问网址	net:www.baidu.com
#define		HTTP_GET			"get:"			// HTTP get 请求
#define		MQTT_CONNET			"mqttcon"		// MQTT CONNECT报文
#define		MQTT_DISCON			"mqttdis"			// MQTT DISCONNECT
#define		MQTT_PING			"ping"			// PING
#define		MQTT_SUB			"subsend:"
#define		MQTT_UNSUB			"unsub:"
#define		MQTT_PUBLISH		"publish:"


/* 函数 */

void Str_DelSpace(char* str);						// 字符串去掉空格，生成新的字符串
bool CampareStrInHeadWithSpace(char* str,char* cmd);// 首部匹配字符串,并用空格替代匹配相同的字符
void RunNetCmd(char* str);							// 根据命令执行不同的网络命令


#endif
