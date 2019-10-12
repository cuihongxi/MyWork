/***
 *  2019��7��20��13:21:34
 *  MQTTЭ��
 *  �޺���
 */
#ifndef __myMQTT_H
#define __myMQTT_H

/**
 * ��ֲ���
 */
#include "uhead.h"
#include "mem.h"

#define	 	my_malloc 	os_zalloc		//��̬�ڴ�����
#define	 	myfree	    os_free			//�ͷ�
#define		myrandom	os_random		//���������


#define	 	mymalloc	Clearmalloc		//��̬���벢�����ڴ�
#define	 	mymemcpy	os_memcpy		//�ڴ濽��
/*
 *
 *
    �̶���ͷ���������� + ʣ�೤��
    �ɱ䱨ͷ��Э��������2�ֽ� + Э���� + Э�鼶�� + ���ӱ�־ + �ͱ�������
    ��Ч�غɣ����밴���˳����֣��ͻ��˱�ʶ�����������⣬������Ϣ���û���������
 */
#define MQTT_Protocol_Level             0x04            //Э�鼶��
/**
 * MQTT���Ʊ��ĵ����ͣ�4~7λ | 0~3λ
 */
typedef enum{
    Reserved = 0,   //����                                            ��Ч�غ�           ���ı�ʶ���ֶ�
    CONNECT = 1<<4,        //�ͻ����������ӷ����                         ��Ҫ               ����Ҫ        
    CONNACK = 2<<4,        //����˵��ͻ���,���ӱ���ȷ��                  ����Ҫ              ����Ҫ
    PUBLISH = 3<<4,        //������������,������Ϣ                       ��ѡ             ��Ҫ�����QoS > 0��
    PUBACK = 4<<4,         //������������,QOS1��Ϣ�����յ�ȷ��           ����Ҫ              ��Ҫ
    PUBREC = 5<<4,         //������������,�����յ�(��֤������һ��)        ����Ҫ              ��Ҫ
    PUBREL = (6<<4)|0x02,  //������������,�����ͷ�(��֤������2��)         ����Ҫ              ��Ҫ
    PUBCOMP = 7<<4,        //������������,QOS2��Ϣ�������(��֤������3��)  ����Ҫ              ��Ҫ
    SUBSCRIBE = (8<<4)|0x02,//�ͻ��˵������,�ͻ��˶�������                  ��Ҫ               ��Ҫ
    SUBACK = 9<<4,         //����˵��ͻ���,����������ȷ��                 ��Ҫ                ��Ҫ
    UNSUBSCRIBE = (10<<4)|0x02, //�ͻ��˵������,�ͻ���ȡ����������          ��Ҫ                ��Ҫ
    UNSUBACK = 11<<4,       //����˵��ͻ���,ȡ�����ı���ȷ��                ����Ҫ              ��Ҫ
    PINGREQ = 12<<4,        //�ͻ��˵������,��������                       ����Ҫ              ����Ҫ
    PINGRESP = 13<<4,       //����˵��ͻ���,������Ӧ                       ����Ҫ              ����Ҫ
    DISCONNECT = 14<<4,     //�ͻ��˵������,�ͻ��˶Ͽ�����                  ����Ҫ              ����Ҫ
    Reserved15 = 15<<4,     //����

}myMQTT_ControlType;



//���ӱ�־ Connect Flags
#define ConnectFlags_Reserved           0x01   //����Ϊ0
#define ConnectFlags_CleanSession       0x02   //����Ự��1������0������
#define ConnectFlags_WillFlag           0x04   //������־��1��ʾ���������ӹر�ʱ������˷���������Ϣ
#define ConnectFlags_WillQoS0           0x08   //����QoSL��ָ������������Ϣʱʹ�õķ��������ȼ�,\
                                                ���������־������Ϊ0������QoSҲ��������Ϊ0(0x00) 
#define ConnectFlags_WillQoS1           0x10   //����QoSH��ָ������������Ϣʱʹ�õķ��������ȼ�,\
                                                ���������־������Ϊ0������QoSҲ��������Ϊ0(0x00) 
#define ConnectFlags_WillRetain         0x20   //��������:���������Ϣ������ʱ��Ҫ����.\
                                                ���������־������Ϊ0:����������־Ҳ��������Ϊ0.\
                                                ���������־������Ϊ1��\
                                                �����������������Ϊ0������˱��뽫������Ϣ�����Ǳ�����Ϣ����\
                                                �����������������Ϊ1������˱��뽫������Ϣ����������Ϣ����

#define ConnectFlags_PassWordFlag       0x40   //�����־
#define ConnectFlags_UserNameFlag       0x80   //�û�����־

/***
 * �̶���ͷ�ṹ��
 */
typedef struct{
	myMQTT_ControlType control;		//��������
	u8 leftNum[4];					//ʣ�೤�ȣ�128����
}FixedHeaderStr;

/***
 * �ɱ䱨ͷ�ṹ��
 */
typedef struct{
	u8* pdat;			//ָ��ɱ䱨ͷ����
	u32 length;			//�ɱ䱨ͷ����
}VariableHeaderStr;

/***
 * ��Ч�غɽṹ��
 */
typedef struct{
	u8* pdat;			//ָ����Ч�غ�����
	u32 length;			//��Ч�غɳ���
}PayloadStr;

/***
 * ���Ʊ��Ľṹ��
 */
typedef struct{
	FixedHeaderStr 		fixHeader;
	VariableHeaderStr 	variableHeader;
	PayloadStr 			payload;

}ControlStr;

/***
 * ������֯��Ľṹ��
 */

typedef struct{
	u8* pdat;			//ָ����Ч�غ�����
	u32 length;			//��Ч����
}DataMessageStr;


/**
 * �Ự�ṹ��
 */
typedef struct{
	u8* wwwDNS;				//��ַ
	u16 port;				//�˿ں�
	u8  protocolLevel ;		//Э�鼶��
	u8* usrName;
	u8* passWord;
	u8  connectFlags;		//���ӱ�־
	u16 keepAlive;			//�������ӣ���
	u8* clientId;			//�ͻ��˱�ʶ�� ��0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ��
	u16 clientIdnum;		//�ͻ��˱�ʶ������

}SessionStr;




/**
 * ������Ϣ PUBLISH:
 *
 *
 */
//Flag Bits��0~3λ
#define PUBLISH_DUP         0X08        //������Ϣ���Ʊ���,�ظ��ַ���־,0 ��ʾ���ǿͻ��˻����˵�һ�����������PUBLISH����
#define PUBLISH_QoS1        0X04        //������Ϣ���Ʊ���,���ĵķ��������ȼ���־,QoS0���ַ�һ��,QoS1���ٷ���һ��
#define PUBLISH_QoS0        0X02        //������Ϣ���Ʊ���,���ĵķ��������ȼ���־
#define	PUBLISH_RETAIN		0x01		//������־,1 ����������Ϊ�ʺ���Ϣ
/**
 * ����
 */
u8* IntTo128(u32 num ,u8* array);   							// �任��128���ƴ洢

// ����һ���»Ự
SessionStr* myMQTT_CreatNewSessionStr(const char* name,const char* password,const char* dns,u16 port);
ControlStr* myMQTT_MallocCONNECTMessage(SessionStr* ss);		// �������ӱ���CONNECT
DataMessageStr*  myMQTT_GetControlMessage(ControlStr* message);	// ��֯���Ϳ��Ʊ�������


#endif


