#include "myMQTT_APP.h"
#include "myWifi.h"



// 会话的CONNECT报文
void ICACHE_FLASH_ATTR myMQTTConnect(SessionStr* ss)
{
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	myMQTT_SendtoServer(ss);
}

// 添加订阅主题报文
void ICACHE_FLASH_ATTR myMQTTAddSubscribe(SessionStr* ss,char* sub,u8 reqQos)
{
	//查找报文标识符链表中有没有该订阅
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(CampareString(sub,SingeListGetnode(idNodeStr,nod)->sub.subname))
			{
				debug("id链表中已存在的主题\r\n");
				SingleList_MoveEndNode(ss->idList,nod);
				return;
			}
	}
	if(nod == 0)	//说明这是一个新主题
	{
		debug("这是一个新主题\r\n");
		idNodeStr* su = (idNodeStr*)malloc(sizeof(idNodeStr));
		su->sub.subname = (u8*)malloc(1 + strlen(sub));
		memcpy(su->sub.subname,sub,strlen(sub));		// 保存字符串
		su->id = ss->id;
		su->sub.reqQos = reqQos;
		SingleList_InsertEnd(ss->idList,su);			// 将该报文添加到备发送的链表中
	}

}

// 订阅主题报文
void ICACHE_FLASH_ATTR myMQTTSubscribe(SessionStr* ss,char* sub,u8 reqQos)
{
	myMQTTAddSubscribe(ss,sub,reqQos);
	ss->messageType = SUBSCRIBE;
	ss->FixPayload = FixSubscribePayload;
	ss->FixVariableHeader = FixSubscribeVariableHeader;
	myMQTT_SendtoServer(ss);
}

// 取消订阅报文
void ICACHE_FLASH_ATTR myMQTTUnSubscribe(SessionStr* ss,char* sub)
{
	myMQTTAddSubscribe(ss,sub,0);
	ss->messageType = UNSUBSCRIBE;
	ss->FixPayload = FixUnSubscribePayload;
	ss->FixVariableHeader = FixUnSubscribeVariableHeader;
	myMQTT_SendtoServer(ss);

}
/***
 * 初始化一个会话
 */
void ICACHE_FLASH_ATTR  myMQTT_SessionStrDefaultInit(SessionStr* ss,char* url,u16 port, char* username, char* password, char* clientId)
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
	ss->espconn = &ST_NetCon;
	ss->subList = NewSingleList();
	ss->idList = NewSingleList();
	ss->Subscribe = myMQTTSubscribe;
	ss->Unsub = myMQTTUnSubscribe;
}

//判断服务器回复的是不是之前发生的报文回复
// 返回真，代表是之前的报文回复
bool ICACHE_FLASH_ATTR Juge_MessageType(SessionStr* ss,u8 dat)
{
	return ((ss->messageType >> 4) +1  == (dat >>4));
}
// CONNECT报文，服务器回复回调函数
void ICACHE_FLASH_ATTR Connack_OkCB(){debug("Connack：ok\n");}
void ICACHE_FLASH_ATTR Connack_ErrorPLCB(){debug("Connack：Error PL\n");}
void ICACHE_FLASH_ATTR Connack_ErrorClientID(){debug("Connack：Error ClientID\n");}
void ICACHE_FLASH_ATTR Connack_ErrorServiceCB(){debug("Connack_Error Service\n");}
void ICACHE_FLASH_ATTR Connack_ErrorUserNameCB(){debug("Connack_Error UserName\n");}
void ICACHE_FLASH_ATTR Connack_ErrorAuthCB(){debug("Connack：Error Auth\n");}


//订阅成功,利用ID来查找
void SubscribeSuccessfule(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
			{
				SingleListNode* nodsub = (SingleListNode*)ss->subList;
				while(SingleList_Iterator(&nodsub))		// 查询主题链表下有没有相同的主题
				{
					if(CampareString(SingeListGetnode(subStr,nodsub)->subname, SingeListGetnode(idNodeStr,nod)->sub.subname))
					{
						debug("已经订阅过的主题\r\n");
						//释放空间
						free(SingeListGetnode(idNodeStr,nod)->sub.subname);
						SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id链表中删除该主题
						return;
					}
				}
				if(nodsub == 0)	// 这是一个新主题
				{
					debug("这是一个新主题\r\n");
					subStr* su = (subStr*)malloc(sizeof(subStr));
					su->subname = SingeListGetnode(idNodeStr,nod)->sub.subname;
					su->reqQos = SingeListGetnode(idNodeStr,nod)->sub.reqQos;
					SingleList_InsertEnd(ss->subList,su);								// 将该报文添加到主题链表中
					SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));	// id链表中删除该主题
				}
			}
	}
}

//释放报文链表节点的空间
void FreeIDLinkenode(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
			{
				//debug("报文标识符，释放空间\r\n");
				free(SingeListGetnode(idNodeStr,nod)->sub.subname);
				SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id链表中删除该主题
			}
	}
}

// 取消订阅成功回调
void UnSubscribeCallBack(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
		{
			SingleListNode* nodsub = (SingleListNode*)ss->subList;
			while(SingleList_Iterator(&nodsub))		// 查询主题链表下有没有相同的主题
			{
				if(CampareString(SingeListGetnode(subStr,nodsub)->subname, SingeListGetnode(idNodeStr,nod)->sub.subname))
				{
					free(SingeListGetnode(subStr,nodsub)->subname);
					SingleList_DeleteNode(ss->subList, SingeListGetnode(subStr,nodsub));// sub链表中删除该主题
					break;
				}
			}
			//释放空间
			free(SingeListGetnode(idNodeStr,nod)->sub.subname);
			SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id链表中删除该主题
		}
	}
}
// 服务器回复回调函数
void ICACHE_FLASH_ATTR myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len)
{
	u8 lenbyte = 0;
	if(len != Change128ToInt((u8*)&pdata[1],&lenbyte) + 2) return;	// 首先校验数据长度对不对
	if(Juge_MessageType(ss,pdata[0]) == FALSE) return;				// 判断收到的回复报文类型
	switch(pdata[0]){
		case CONNACK:
			switch(pdata[3])		// 处理连接返回码
			{
				case CONNACK_OK:  				Connack_OkCB();					break;
				case CONNACK_ERROR_PL:			Connack_ErrorPLCB();			break;
				case CONNACK_ERROR_CLIENTID:	Connack_ErrorClientID();		break;
				case CONNACK_ERROR_SERVICE:		Connack_ErrorServiceCB();		break;
				case CONNACK_ERROR_USERNAME:	Connack_ErrorUserNameCB();		break;
				case CONNACK_ERROR_AUTH:		Connack_ErrorAuthCB();			break;
			}
			break;
		case PINGRESP:
			if(pdata[1] == 0) debug("收到PING回复...\r\n");
			else debug("PING回复出错...\r\n");
			break;
		case SUBACK:	//订阅主题回复
			if(pdata[4] == 0x80)
				{
					debug("订阅报文：%d 失败 \r\n",GetU16LittleEnding(&pdata[2]));//订阅失败
					FreeIDLinkenode(ss,GetU16LittleEnding(&pdata[2]));
				}
			else
				{
					debug("订阅报文：%d 成功 \r\n",GetU16LittleEnding(&pdata[2]));//订阅失败,pdata[3]|((u16)pdata[2]<<8)
					SubscribeSuccessfule(ss,GetU16LittleEnding(&pdata[2]));
				}
			break;
		case UNSUBACK:	//取消订阅主题回复
				debug("取消报文ID：%d\r\n",GetU16LittleEnding(&pdata[2]));
				UnSubscribeCallBack(ss,GetU16LittleEnding(&pdata[2]));
	}

}
