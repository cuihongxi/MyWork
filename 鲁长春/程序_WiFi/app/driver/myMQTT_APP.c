#include "myMQTT_APP.h"
#include "myWifi.h"

// 会话的CONNECT报文
void myMQTTConnect(SessionStr* ss)
{
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	ControlStr* cs = myMQTT_CreatMessage(ss);								// 创建报文
	DataMessageStr*  ds = myMQTT_CreatControlMessage(cs);					// 组织发送控制报文数据
	ESP8266_SendtoService(ds->pdat,ds->length);								// 向网络发送数据
}

/***
 * 初始化一个会话
 */
void ICACHE_FLASH_ATTR  myMQTT_SessionStrInit(SessionStr* ss,char* url,u16 port, char* username, char* password, char* clientId)
{
	ss->url = url;												// 网址
	ss->port = port;											// 端口号
	ss->usrName = username;
	ss->passWord = password;
	ss->keepAlivetime = KEEPALIVE_SEC;
	ss->clientId = clientId;
	ss->connectFlags = CONNECT_FLAG_PARA;			//用户名密码+清除会话 必须按这个顺序出现：客户端标识符，遗嘱主题，遗嘱消息，用户名，密码
	ss->protocolLevel = MQTT_Protocol_Level;
	ss->DisConnect = myMQTT_Disconnect;
	ss->KeepAlive = myMQTT_Ping;
	ss->Connect = myMQTTConnect;
}

