/**
 * 2019��10��22��09:35:37
 * ����myMQTTӦ�ò�ĺ���
 * ʹ�÷�����
 * 	myMQTT_SessionStrDefaultInit(������)������ʼ���ỰSessionStr* ss��
 * 	���Ǹú���һ�㲻�ᱻ�û�ֱ��ʹ�ã�ͨ�����ɰ����ƻ����������Ƴ�ʼ��ʱ���á�
 * 	���ļ������˿��Ʊ��ĵ����к�������ػص���������ע����Ự�ṹ��ss��
 * 	�����ԵĻص�����������Ӧ���������λ���Ա������Ӧ���ܡ�
 */

#ifndef		__myMQTT_APP_H
#define		__myMQTT_APP_H
#include "uhead.h"
#include "myMQTT.h"




// ����һ���»Ự
void myMQTT_SessionStrDefaultInit(SessionStr* ss,char* url,u16 port, char* name, char* password, char* clientId);


#endif

