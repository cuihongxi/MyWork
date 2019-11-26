#include "myMQTT_APP.h"
#include "myWifi.h"



// �Ự��CONNECT����
void ICACHE_FLASH_ATTR myMQTTConnect(SessionStr* ss)
{

	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	ControlStr* cs = myMQTT_CreatMessage(ss);								// ��������
	DataMessageStr*  ds = myMQTT_CreatControlMessage(cs);					// ��֯���Ϳ��Ʊ�������
	ESP8266_SendtoService(ds->pdat,ds->length);								// ����ɹ���ȡ��IP���������緢������
}

// ��Ӷ������ⱨ��
void ICACHE_FLASH_ATTR myMQTTAddSubscribe(SessionStr* ss,char* sub,u8 reqQos)
{
	subStr* su = (subStr*)malloc(1 + strlen(sub));
	memcpy(su->subname,sub,strlen(sub));
	su->reqQos = reqQos;
	SingleList_InsertEnd(ss->subList,su);
}

// �������ⱨ��
void ICACHE_FLASH_ATTR myMQTTSubscribe(SessionStr* ss)
{

	ss->messageType = SUBSCRIBE;
	ss->FixPayload = FixSubscribePayload;
	ss->FixVariableHeader = FixSubscribeVariableHeader;
	ControlStr* cs = myMQTT_CreatMessage(ss);								// ��������
	DataMessageStr*  ds = myMQTT_CreatControlMessage(cs);					// ��֯���Ϳ��Ʊ�������
	ESP8266_SendtoService(ds->pdat,ds->length);								// ����ɹ���ȡ��IP���������緢������
}

/***
 * ��ʼ��һ���Ự
 */
void ICACHE_FLASH_ATTR  myMQTT_SessionStrDefaultInit(SessionStr* ss,char* url,u16 port, char* username, char* password, char* clientId)
{
	ss->url = url;												// ��ַ
	ss->port = port;											// �˿ں�
	ss->usrName = username;
	ss->passWord = password;
	ss->keepAlivetime = KEEPALIVE_SEC;
	ss->clientId = clientId;
	ss->connectFlags = CONNECT_FLAG_PARA;			//�û�������+����Ự ���밴���˳����֣��ͻ��˱�ʶ�����������⣬������Ϣ���û���������
	ss->protocolLevel = MQTT_Protocol_Level;
	ss->DisConnect = myMQTT_Disconnect;
	ss->KeepAlive = myMQTT_Ping;
	ss->Connect = myMQTTConnect;
	ss->espconn = &ST_NetCon;
	ss->subList = NewSingleList();
	ss->AddSubscribe = myMQTTAddSubscribe;
//	ss->Subscribe =
}

