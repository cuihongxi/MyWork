/**
 * 2019年11月30日02:02:22
 * 订阅主题程序流程：
 * 		发送新主题时，将该主题添加到报文标识符链表idList中。
 * 		发送主题时，检测主题报文标识符链表下有没有该主题，如果有，则将他移动到链表末端，以便后续处理发送。
 * 		收到服务器回复，当订阅失败，从报文标识符链表中删除该节点，并释放空间。成功时从报文标识符链表中摘下，并添加到主题链表下。
 * 	取消订阅主题程序流程：
 * 		把他当成主题处理。如果没有这个主题则添加到报文标识符链表中。
 * 		收到服务器回复，查找报文标识符链表中相同ID的报文，获取主题名。查找主题链表下相同主题名的报文，将两个链表下这个节点的信息删除。
 * 		如果主题链表下没有这个主题名，则只删除报文标识符链表下的节点
 * 	Publish发布流程：
 * 			Qos0: 将该等级保存到报文链表中，在接下来的处理数据流程里 ，直接发生消息，并删除节点
 * 			Qos1：先将此消息添加到报文标识符链表下，再进行发送。收到服务器回复后再将该节点删除。
 * 			Qos2: 暂不处理
 *
 */
#include "myMQTT_APP.h"
#include "myWifi.h"


void myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len);// 服务器回复回调函数

// 会话的CONNECT报文
void ICACHE_FLASH_ATTR myMQTTConnect(SessionStr* ss)
{
	ss->erro = ERRO_OK;
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	myMQTT_SendtoServer(ss);
}

// 添加订阅主题报文
void ICACHE_FLASH_ATTR myMQTTAddSubscribe(SessionStr* ss,char* sub,enumQos reqQos)
{
	//查找报文标识符链表中有没有该订阅
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(CampareString(sub,((subStr*)(SingeListGetnode(idNodeStr,nod)->mes))->sub))
			{
				debug("id链表中已存在的主题\r\n");
				SingleList_MoveEndNode(ss->idList,nod);
				return;
			}
	}
	if(nod == 0)	//说明这是一个新信息
	{
		debug("这是一个新主题\r\n");
		idNodeStr* idnod = (idNodeStr*)malloc(sizeof(idNodeStr));
		if(idnod == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		subStr* su = (subStr*)malloc(sizeof(subStr));
		if(su == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		su->sub = (u8*)malloc(1 + strlen(sub));
		if(su->sub == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		memcpy(su->sub,sub,strlen(sub));				// 保存字符串
		su->reqQos = reqQos;
		idnod->id = ss->id;
		idnod->mes = su;
		SingleList_InsertEnd(ss->idList,idnod);			// 将该报文添加到备发送的链表中
	}

}

// 订阅主题报文
void ICACHE_FLASH_ATTR myMQTTSubscribe(SessionStr* ss,char* sub,enumQos reqQos)
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

// Publish报文添加数据
void ICACHE_FLASH_ATTR myMQTTAddPublish(SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup)
{
	// 根据消息等级来组织数据
   if(reqQos & PUBLISH_QoS2)
		{
			debug("这是Qos2的报文，暂时没有相关处理函数\r\n");
			ss->erro |= ERRO_QoS2;
			return;
		}
	else
	{
		idNodeStr* idnod = (idNodeStr*)malloc(sizeof(idNodeStr));
		if(idnod == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		pubStr* pu = (pubStr*)malloc(sizeof(pubStr));
		if(pu == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		pu->pub = (u8*)malloc(1 + strlen(pubdata));
		if(pu->pub == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		pu->sub = (u8*)malloc(1 + strlen(sub));
		if(pu->sub == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}

		memcpy(pu->sub,sub,strlen(sub));						// 保存字符串
		memcpy(pu->pub,pubdata,strlen(pubdata));				// 保存字符串
		pu->reqQos = reqQos;
		pu->retain = retain;
		pu->dup = dup;
		idnod->id = ss->id;
		idnod->mes = pu;
		SingleList_InsertEnd(ss->idList,idnod);					// 将该报文添加到备发送的链表中
	}




}
// Publish报文
void ICACHE_FLASH_ATTR myMQTTPublish(SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup)
{
	myMQTTAddPublish(ss,sub,pubdata,reqQos,retain,dup);
	ss->messageType = PUBLISH;
	ss->FixPayload = FixPublishPayload;
	ss->FixVariableHeader = FixPublishVariableHeader;
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
	ss->ServerCB = myMQTT_ServerReplyCB;
	ss->Publish = myMQTTPublish;
	ss->erro = ERRO_OK;

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
				subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
				SingleListNode* nodsub = (SingleListNode*)ss->subList;
				while(SingleList_Iterator(&nodsub))		// 查询主题链表下有没有相同的主题
				{
					if(CampareString(SingeListGetnode(subStr,nodsub)->sub,su->sub))
					{
						debug("已经订阅过的主题,释放id链表节点\r\n");
						FreeSubnodInLink(ss,ss->idList,nod);//释放空间
						return;
					}
				}
				if(nodsub == 0)	// 这是一个新主题
				{
					debug("这是一个新主题\r\n");
					subStr* su = (subStr*)SingeListGetnode(idNodeStr,nod)->mes;
					SingleList_InsertEnd(ss->subList,su);								// 将该报文添加到主题链表中
					SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));	// id链表中删除该主题
				}
			}
	}
}

//释放报文链表节点的空间
void ICACHE_FLASH_ATTR FreeIDLinkenode(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
			{
				//释放idlinke链表下的主题节点
				 FreeSubnodInLink(ss,ss->idList, nod);
			}
	}
}

// 取消订阅成功回调
void ICACHE_FLASH_ATTR UnSubscribeCallBack(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
		{
			SingleListNode* nodsub = (SingleListNode*)ss->subList;
			while(SingleList_Iterator(&nodsub))		// 查询主题链表下有没有相同的主题
			{
				if(CampareString(SingeListGetnode(subStr,nodsub)->sub, ((subStr*)SingeListGetnode(idNodeStr,nod)->mes)->sub))
				{
					SingleList_DeleteNode(ss->subList, SingeListGetnode(subStr,nodsub));// sub链表中删除该主题
					break;
				}
			}
			//释放空间
			FreeSubnodInLink(ss,ss->idList,nod);// id链表中删除该主题
		}
	}
}

//publish报文回复收到,利用ID来查找
void PublishSuccessfule(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// 查找相同ID
			{
				FreePubnodInIdLink(ss,nod);	// 释放节点
			}
	}
}
// 服务器回复回调函数
void ICACHE_FLASH_ATTR myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len)
{
	u8 lenbyte = 0;
	if(len != Change128ToInt((u8*)&pdata[1],&lenbyte) + lenbyte + 1) return;	// 首先校验数据长度对不对
	switch(pdata[0]&0xf0){
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
			break;
		case PUBACK:	// pulish 报文回复
				debug("收到publis回复 ID:%d\r\n",GetU16LittleEnding(&pdata[2]));
				PublishSuccessfule(ss,GetU16LittleEnding(&pdata[2]));
			break;
		case PUBLISH:	// pulish 报文
				debug("收到服务器的推送：\r\n");
				pubStr* pub = myMQTT_PublishCB(ss,pdata,len);
				debug("主题：%s\r\n",pub->sub);
				debug("内容：%s\r\n",pub->pub);
				free(pub->pub);
				free(pub->sub);
				free(pub);
			break;
	}
}


