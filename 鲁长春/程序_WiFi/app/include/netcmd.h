/**
 * �����ַ�����������Ĵ���
 * 2019��10��13��07:50:16
 */

#ifndef		__netcmd_h
#define		__netcmd_h

#include "uhead.h"

#define		TCP_CONNECT			"tcpcon:"		// TCP_CLIENTģʽ���ӷ���������ʼͷ  tcp_c:192.168.31.67:6666
#define		MESSAGE				"mes:"			// ��Ϣ��ʼ����ʼͷ mes:ni hao a
#define		DIS_CONNECT			"disconnect"	// �Ͽ���TCP_SERVER������
#define		NET_CONNECT			"net:"			// ͨ������������ַ	net:www.baidu.com
#define		HTTP_GET			"get:"			// HTTP get ����
#define		MQTT_CONNET			"mqttcon"		// MQTT CONNECT����
#define		MQTT_DISCON			"mqttdis"			// MQTT DISCONNECT
#define		MQTT_PING			"ping"			// PING
#define		MQTT_SUB			"subsend:"
#define		MQTT_UNSUB			"unsub:"
#define		MQTT_PUBLISH		"publish:"


/* ���� */

void Str_DelSpace(char* str);						// �ַ���ȥ���ո������µ��ַ���
bool CampareStrInHeadWithSpace(char* str,char* cmd);// �ײ�ƥ���ַ���,���ÿո����ƥ����ͬ���ַ�
void RunNetCmd(char* str);							// ��������ִ�в�ͬ����������


#endif
