/**
 * �����ַ�����������Ĵ���
 * 2019��10��13��07:50:16
 */

#ifndef		__netcmd_h
#define		__netcmd_h

#include "uhead.h"

#define		TCP_CONNECT			"tcp_c:"		// TCP_CLIENTģʽ���ӷ���������ʼͷ  tcp_c:192.168.31.67:6666
#define		MESSAGE				"mes:"			// ��Ϣ��ʼ����ʼͷ mes:ni hao a
#define		DIS_CONNECT			"disconnect"	// �Ͽ���TCP_SERVER������
#define		NET_CONNECT			"net:"			// ͨ������������ַ	net:www.baidu.com

/* ���� */

void Str_DelSpace(char* str);						// �ַ���ȥ���ո������µ��ַ���
bool CampareStrInHeadWithSpace(char* str,char* cmd);// �ײ�ƥ���ַ���,���ÿո����ƥ����ͬ���ַ�
void RunNetCmd(char* str);							// ��������ִ�в�ͬ����������


#endif
