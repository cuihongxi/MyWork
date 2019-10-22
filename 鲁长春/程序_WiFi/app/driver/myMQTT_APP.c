#include "myMQTT_APP.h"
#include "myWifi.h"

// �Ự��CONNECT����
void myMQTTConnect(SessionStr* ss)
{
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	ControlStr* cs = myMQTT_CreatMessage(ss);								// ��������
	DataMessageStr*  ds = myMQTT_CreatControlMessage(cs);					// ��֯���Ϳ��Ʊ�������
	ESP8266_SendtoService(ds->pdat,ds->length);								// �����緢������
}

/***
 * ��ʼ��һ���Ự
 */
void ICACHE_FLASH_ATTR  myMQTT_SessionStrInit(SessionStr* ss,char* url,u16 port, char* username, char* password, char* clientId)
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
}

