/**
 * 2019��10��16��09:59:59
 * �����ƹ���MQTT
 * ���ļ������˰�������ز���
 * ʹ�÷�����
 * 		ʹ�� ss = (SessionStr*)ConnectAliyunMqtt(...) ����ʼ���Ự�ṹ��ss
 */


#ifndef	__aliyunMQTT_h
#define	__aliyunMQTT_h
#include "uhead.h"
#include "myMQTT.h"

#define		PASSWORD_MIN_LEN		20	//�������볤��



// �����ƽṹ��
typedef struct _AliyunStr{
	SessionStr 		ss;					// �Ự�ṹ��
	char* 			productKey;			//
	char*			deviceName;
	char*			deviceSecret;		// ��Կ
	char* 			clientId;			// Ϊ�豸��ID��Ϣ����ȡ����ֵ��������64�ַ����ڡ�����ʹ���豸��MAC��ַ��SN�롣


}AliyunStr;

AliyunStr*  ConnectAliyunMqtt(char* url,u16 port,AliyunStr* as);	// ���������Ƶ�MQTT�Ự

#endif

