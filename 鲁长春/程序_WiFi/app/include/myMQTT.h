/***
 *  2019��7��20��13:21:34
 *  MQTTЭ��
 *  �޺���
 *  2019��11��30��21:56:51 ����ᣩ
 *  ���ļ������⿪�ţ�ֻ��myMQTT_APP�ļ��б�����
 */
#ifndef __myMQTT_H
#define __myMQTT_H

/**
 * ��ֲ���
 */
#include "uhead.h"
#include "mem.h"
#include "SingleList.h"

#define	 	my_malloc 	os_zalloc		//��̬�ڴ�����
#define	 	myfree	    os_free			//�ͷ�
#define		myrandom	os_random		//���������
#define	 	mymalloc	Clearmalloc		//��̬���벢�����ڴ�
/*
 *
 *
    �̶���ͷ���������� + ʣ�೤��
    �ɱ䱨ͷ��Э��������2�ֽ� + Э���� + Э�鼶�� + ���ӱ�־ + �ͱ�������
    ��Ч�غɣ����밴���˳����֣��ͻ��˱�ʶ�����������⣬������Ϣ���û���������
 */

/**
 * MQTT���Ʊ��ĵ����ͣ�4~7λ | 0~3λ
 */
typedef enum{
    Reserved = 0,   			//����                                            					��Ч�غ�           	���ı�ʶ���ֶ�
    CONNECT = 1<<4,        		//�ͻ����������ӷ����                        				 ��Ҫ               	����Ҫ
    CONNACK = 2<<4,        		//����˵��ͻ���,���ӱ���ȷ��                  			����Ҫ              	����Ҫ
    PUBLISH = 3<<4,        		//������������,������Ϣ                       			��ѡ             	��Ҫ�����QoS > 0��
    PUBACK = 4<<4,         		//������������,QOS1��Ϣ�����յ�ȷ��       		����Ҫ              	��Ҫ
    PUBREC = 5<<4,         		//������������,�����յ�(��֤������һ��)        ����Ҫ              	��Ҫ
    PUBREL = (6<<4)|0x02,  		//������������,�����ͷ�(��֤������2��)         ����Ҫ             	 ��Ҫ
    PUBCOMP = 7<<4,        		//������������,QOS2��Ϣ�������(��֤������3��)  ����Ҫ             	 ��Ҫ
    SUBSCRIBE = (8<<4)|0x02,	//�ͻ��˵������,�ͻ��˶�������                  		 	 ��Ҫ               	��Ҫ
    SUBACK = 9<<4,         		//����˵��ͻ���,����������ȷ��                			 ��Ҫ               	 ��Ҫ
    UNSUBSCRIBE = (10<<4)|0x02, //�ͻ��˵������,�ͻ���ȡ����������          			��Ҫ                	��Ҫ
    UNSUBACK = 11<<4,       	//����˵��ͻ���,ȡ�����ı���ȷ��                			����Ҫ             	 ��Ҫ
    PINGREQ = 12<<4,        	//�ͻ��˵������,��������                       			����Ҫ              	����Ҫ
    PINGRESP = 13<<4,       	//����˵��ͻ���,������Ӧ                       			����Ҫ              	����Ҫ
    DISCONNECT = 14<<4,     	//�ͻ��˵������,�ͻ��˶Ͽ�����                  			����Ҫ              	����Ҫ
    Reserved15 = 15<<4,     	//����

}myMQTT_ControlType;

typedef	u16	IDTYPE;				// ���ı�ʶ��������

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
// ���ӷ�����
#define		CONNACK_OK						0	// ����OK
#define		CONNACK_ERROR_PL				1	// ��֧�ֵ�Э��汾
#define		CONNACK_ERROR_CLIENTID			2	// ���ϸ�Ŀͻ��˱�ʶ��
#define		CONNACK_ERROR_SERVICE			3	// ������������
#define		CONNACK_ERROR_USERNAME			4	// �û������������
#define		CONNACK_ERROR_AUTH				5	// �ͻ���δ����Ȩ


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
	u8* pdat;				//ָ��ɱ䱨ͷ����
	u32 length;				//�ɱ䱨ͷ����
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
	FixedHeaderStr 		fixHeader;				// �̶���ͷ���������� + ʣ�೤��
	VariableHeaderStr 	variableHeader;			// �ɱ䱨ͷ
	PayloadStr 			payload;				// ��Ч�غ�

}ControlStr;

/***
 * ������֯��Ľṹ��
 */

typedef struct{
	u8* pdat;			//ָ����Ч�غ�����
	u32 length;			//��Ч����
}DataMessageStr;

typedef enum{
	 PUBLISH_QoS0 = 0,        	//������Ϣ���Ʊ���,���ĵķ��������ȼ���־
	 PUBLISH_QoS2 = 0X04,       //������Ϣ���Ʊ���,���ĵķ��������ȼ���־,QoS0���ַ�һ��,QoS1���ٷ���һ��
	 PUBLISH_QoS1 = 0X02,       //������Ϣ���Ʊ���,���ĵķ��������ȼ���־
}enumQos;

typedef struct{
	 u8* 		sub;		// �ַ���
	 enumQos 	reqQos;		// ��������Ҫ��
}subStr;				// ��������ṹ��

typedef enum{
	dup_no = 0,
	dup_yes = 0x08,
}enumdup;				// �Ƿ��ط���־

typedef struct{
	 u8* 		sub;		// �����ַ���
	 enumQos 	reqQos;		// ��������Ҫ��
	 u8* 		pub;		// �ַ���
	 bool		retain;		// publish�������Ƿ񱣴���Ϣ
	 enumdup	dup;		// �Ƿ��ط���־
}pubStr;				// publish�ṹ��

typedef struct{
	 void*  mes;		// ��Ϣͷ��ַ
	 IDTYPE id;			// ���ı�ʶ��

}idNodeStr;				// ���ı�ʶ�����ṹ��

typedef	u8	ERROTYPE;

/****�������****/
#define	ERRO_OK			0		// �޴���
#define	ERRO_QoS2		0x04	// ��ʱ����QoS2���д���
#define	ERRO_MALLOC		0x08	// ��̬�ڴ�����ʧ��


/**
 * �Ự�ṹ��
 */
typedef struct _SessionStr{
	struct espconn* 		espconn;			// �������ӽṹ��
	char* 					url;				// ��ַ
	u16 					port;				// �˿ں�
	u8  					protocolLevel ;		// Э�鼶��
	char*  					usrName;
	char*  					passWord;
	u8  					connectFlags;		// ���ӱ�־
	u16 					keepAlivetime;		// �������ӣ���
	char*  					clientId;			// �ͻ��˱�ʶ�� , �ͻ���ID
	myMQTT_ControlType 		messageType;		// ��������
	SingleList*				subList;			// �������������ѳɹ����ĵ�������������������
	SingleList*				idList;				// ���ı�ʶ������,idNodeStr��������������
	IDTYPE					id;					// ���ı�ʶ����ÿ����һ�α���������һ��
	ERROTYPE				erro;				// ����������

	void(*Connect)(struct _SessionStr*)		;			// ���ӱ���
	void(*Disconnect)(struct _SessionStr*)	;			// �Ͽ����ӱ���
	void(*Ping)(struct _SessionStr*)		;			// PING����
	void(*Subscribe)(struct _SessionStr*, char*, enumQos);	// ��������
	void(*Unsub)(struct _SessionStr*,char* sub)		;	// ȡ������
	void(*Puback)(struct _SessionStr*)		;			// ����ȷ��
	void(*KeepAlive)(struct _SessionStr*)					;			// Ping��
	void(*DisConnect)(struct _SessionStr*)					;			// ��������Ͽ�����
	void(*FixVariableHeader)(ControlStr*,struct _SessionStr*);			// ��䱨�Ŀɱ䱨ͷ
	void(*FixPayload)(ControlStr*,struct _SessionStr*);					// ��䱨����Ч�غ�
	void(*ServerCB)(struct _SessionStr* ss,char * pdata, unsigned short len);	// �������ظ��Ļص�����
	void(*Publish)(struct _SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup);// ������Ϣ

}SessionStr;




/**
 * ������Ϣ PUBLISH:
 *
 *
 */
//Flag Bits��0~3λ





/*�û�Ĭ�ϲ���*/
#define	CONNECT_FLAG_PARA	(ConnectFlags_UserNameFlag|ConnectFlags_PassWordFlag|ConnectFlags_CleanSession)	//���ӱ�־����Ĭ��ֵ
#define	KEEPALIVE_SEC					60			// �룬��������ʱ��
#define MQTT_Protocol_Level             0x04        // Э�鼶��



/**
 * ����
 */




ControlStr*  	myMQTT_CreatMessage(SessionStr* ss);						// ��������
void  			FixConnectVariableHeader(ControlStr* cs,SessionStr* ss);	// ���CONNECT���Ŀɱ䱨ͷ
void  			FixConnectPayload(ControlStr* cs,SessionStr* ss);			// ���CONNECT������Ч�غ�
void  			FixSubscribeVariableHeader(ControlStr* cs,SessionStr* ss);	// ���Subscribe���Ŀɱ䱨ͷ
void  			FixSubscribePayload(ControlStr* cs,SessionStr* ss);			// ���Subscribe������Ч�غ�
void  			FixUnSubscribeVariableHeader(ControlStr* cs,SessionStr* ss);// ���UnSubscribe���Ŀɱ䱨ͷ
void  			FixUnSubscribePayload(ControlStr* cs,SessionStr* ss);		// ���UnSubscribe������Ч�غ�
void  			FixPublishVariableHeader(ControlStr* cs,SessionStr* ss);	// ���Publish���Ŀɱ䱨ͷ
void  			FixPublishPayload(ControlStr* cs,SessionStr* ss);			// ���Publish������Ч�غ�

void 			myMQTT_Disconnect();										// �Ͽ�����
void 			myMQTT_Ping();												// ��������PING��

ERROTYPE  		myMQTT_SendtoServer(SessionStr* ss);						// ����MQTT��������

void 			FreeSubnodInLink(SessionStr* ss,SingleList* list,void* nod);	//�ͷ������µ�����ڵ�
void  			FreePubnodInIdLink(SessionStr* ss,void* nod);					//�ͷ�id�����µ�publish�ڵ�
pubStr* 		myMQTT_PublishCB(SessionStr* ss,char * pdata, unsigned short len);	// ���յ�������������,��Ҫ�ͷŷ��صĿռ�

#endif


