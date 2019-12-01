#include "myMQTT.h"
#include "myWifi.h"
#include "mystring.h"

/***
 * 动态申请内存，并将申请的内存全部清零
 */

void* ICACHE_FLASH_ATTR ICACHE_FLASH_ATTR  Clearmalloc(u32 num)
{
	u8 *p = (u8*)my_malloc(num);
	u32 i = 0;
	for(i = 0;i<num;i++) p[i] = 0;
	return (void*)p;

}

// 断开连接
void ICACHE_FLASH_ATTR  myMQTT_Disconnect(struct _SessionStr* ss)
{
	u8 discmd[2] = {0xe0,00};
	ss->messageType = DISCONNECT;
	espconn_send(ss->espconn,discmd,2);
}

// 心跳包，PING包
void ICACHE_FLASH_ATTR  myMQTT_Ping(struct _SessionStr* ss)
{
	u8 pincmd[2] = {0xc0,00};
	ss->messageType = PINGREQ;
	espconn_send(ss->espconn,pincmd,2);
}


//转换成字段，将字符串以字节的形式保存到数组中，并在最前面添加长度
u8*  ICACHE_FLASH_ATTR Str2ByteSector(u8* str , u8* pdat)
{
	u32 len = strlen(str);
	pdat[0] = len >> 8;
	pdat[1] = (u8)len;
	return memcpy(&pdat[2],str,len);
}

// 填充固定报头
void ICACHE_FLASH_ATTR  FixHeader(ControlStr* cs,myMQTT_ControlType type)
{
	cs->fixHeader.control = type;
	IntTo128(cs->payload.length + cs->variableHeader.length,cs->fixHeader.leftNum);
}
/*********************CONNECT报文*****************************/
// 填充CONNECT报文可变报头
void ICACHE_FLASH_ATTR  FixConnectVariableHeader(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	cs->variableHeader.length = 10;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	if(cs->variableHeader.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	cs->variableHeader.pdat[0] = 0;
	cs->variableHeader.pdat[1] = 0x04;
	cs->variableHeader.pdat[2] = 'M';
	cs->variableHeader.pdat[3] = 'Q';
	cs->variableHeader.pdat[4] = 'T';
	cs->variableHeader.pdat[5] = 'T';
	cs->variableHeader.pdat[6] = ss->protocolLevel;
	cs->variableHeader.pdat[7] = ss->connectFlags;
	cs->variableHeader.pdat[8] = ss->keepAlivetime >> 8;
	cs->variableHeader.pdat[9] = (u8)(ss->keepAlivetime);
}

// 填充连接报文有效载荷
void ICACHE_FLASH_ATTR  FixConnectPayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	cs->payload.length = GetStringByteNum(ss->clientId) + GetStringByteNum(ss->usrName) + GetStringByteNum(ss->passWord) +6;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	if(cs->payload.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	Str2ByteSector(ss->clientId ,cs->payload.pdat);
	u8* p = cs->payload.pdat;
	p = p + 2 + GetStringByteNum(ss->clientId);
	Str2ByteSector(ss->usrName ,p);
	p = p + 2 + GetStringByteNum(ss->usrName);
	Str2ByteSector(ss->passWord ,p);
}
/*********************CONNECT报文*****************************/

/*********************Subscribe报文*****************************/
// 填充订阅报文可变报头
void ICACHE_FLASH_ATTR  FixSubscribeVariableHeader(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	cs->variableHeader.length = 2;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	if(cs->variableHeader.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	cs->variableHeader.pdat[0] = (u8)((ss->id)>>8);
	cs->variableHeader.pdat[1] = (u8)(ss->id);
}

// 填充订阅报文有效载荷
void ICACHE_FLASH_ATTR  FixSubscribePayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	u16 len = GetStringByteNum((const char*)(su->sub));
	cs->payload.length = 3 + len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	if(cs->payload.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	Str2ByteSector(su->sub,cs->payload.pdat);
	cs->payload.pdat[cs->payload.length - 1] = su->reqQos;
	ss->id ++;	//报文标识符自增一次
}
/*********************Subscribe报文*****************************/
/*********************UnSubscribe报文*****************************/
// 填充取消订阅报文可变报头
void ICACHE_FLASH_ATTR  FixUnSubscribeVariableHeader(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	cs->variableHeader.length = 2;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	if(cs->variableHeader.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	cs->variableHeader.pdat[0] = (u8)((ss->id)>>8);
	cs->variableHeader.pdat[1] = (u8)(ss->id);
}

// 填充取消订阅报文有效载荷
void ICACHE_FLASH_ATTR  FixUnSubscribePayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	u16 len = GetStringByteNum((const char*)(su->sub));
	cs->payload.length = 2 + len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	if(cs->payload.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	Str2ByteSector(su->sub,cs->payload.pdat);
	ss->id ++;	//报文标识符自增一次

}
/*********************UnSubscribe报文*****************************/
/*********************Publish报文*****************************/
// 填充Publish报文可变报头,主题名+报文标识符
void ICACHE_FLASH_ATTR  FixPublishVariableHeader(ControlStr* cs,SessionStr* ss)
{

	if(ss->erro) return;									// 暂时不处理这个等级的消息
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	pubStr* pub = (pubStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	ss->messageType = PUBLISH|pub->reqQos|pub->retain|pub->dup;

	u16 len = GetStringByteNum((const char*)(pub->sub));
	if(pub->reqQos & PUBLISH_QoS2 )
	{

	}else if(pub->reqQos & PUBLISH_QoS1)
	{
		cs->variableHeader.length = len + 2 + 2;
		cs->variableHeader.pdat = malloc(cs->variableHeader.length);
		if(cs->variableHeader.pdat == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		Str2ByteSector(pub->sub,cs->variableHeader.pdat);
		u8* p = cs->variableHeader.pdat;
		p = p + 2 + GetStringByteNum(pub->sub);
		p[0] = (u8)((ss->id)>>8);	// 添加报文标识符
		p[1] = (u8)(ss->id);

	}else
	{
		cs->variableHeader.length = len + 2;
		cs->variableHeader.pdat = malloc(cs->variableHeader.length);
		if(cs->variableHeader.pdat == 0)
		{
			ss->erro |= ERRO_MALLOC;
			return;
		}
		Str2ByteSector(pub->sub,cs->variableHeader.pdat);
	}
}

// 填充Publish报文有效载荷
void ICACHE_FLASH_ATTR  FixPublishPayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	pubStr* pu = (pubStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	u16 len = GetStringByteNum((const char*)(pu->pub));
	cs->payload.length = len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	if(cs->payload.pdat == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return;
	}
	memcpy(cs->payload.pdat,pu->pub,cs->payload.length);
	ss->id ++;	//报文标识符自增一次
	if(pu->reqQos == 0)	//删除节点，释放空间
	{
		FreePubnodInIdLink(ss,nod);
	}
}
/*********************Publish报文*****************************/
/**
 * 创建报文
 */
ControlStr* ICACHE_FLASH_ATTR  myMQTT_CreatMessage(SessionStr* ss)
{
	ss->erro = ERRO_OK;
	ControlStr* cs = mymalloc(sizeof(ControlStr));
	if(cs == 0)
	{
		ss->erro |= ERRO_MALLOC;
		return 0;
	}
	ss->FixVariableHeader(cs,ss);							// 填充报文可变报头
	ss->FixPayload(cs,ss);									// 填充报文有效载荷
	if(ss->erro ) return cs;
	FixHeader(cs,ss->messageType);	 						// 填充固定报头
	return cs;
}


/**
 * 释放控制报文结构体内存
 */

void ICACHE_FLASH_ATTR  Free_ControlMessage(ControlStr* message)
{
	myfree(message->variableHeader.pdat);	// 释放可变报头结构体
	myfree(message->payload.pdat);			// 释放有效载荷结构体
	myfree(message);						// 释放控制报文结构体
}

/***
 * 组织发送控制报文数据,并释放控制报文动态内存
 * 返回数据报的头
 */

DataMessageStr*  ICACHE_FLASH_ATTR  myMQTT_CreatControlMessage(ControlStr* message)
{
	if(message == 0) return 0;
	DataMessageStr* dataMessage = mymalloc(sizeof(DataMessageStr));
	if(dataMessage == 0) return 0;
	u8 i = 0;
	u8 lenbyte = 0; //需要的字节数
	u32 len = Change128ToInt(message->fixHeader.leftNum,&lenbyte);//获得剩余长度,需要的字节数
	dataMessage->length = len + lenbyte + 1;
	dataMessage->pdat = mymalloc(dataMessage->length);
	if(dataMessage->pdat == 0)return 0;
	u8* pdat = dataMessage->pdat;
	*pdat = message->fixHeader.control;	//拷贝文件类型

	while(i<4)							//拷贝剩余长度
	{
		pdat ++;
		*pdat = message->fixHeader.leftNum[i];
		if(*pdat & 0x80) i++;
		else break;
	}
	pdat ++;
	//拷贝可变报头
	memcpy(pdat,message->variableHeader.pdat,message->variableHeader.length);
	pdat += message->variableHeader.length;
	//拷贝有效载荷
	memcpy(pdat,message->payload.pdat,message->payload.length);

/*	debug("\r\nMQTT:\r\n");
	u16 j = 0;
	for( j = 0; j<dataMessage->length;j++)
	{
			debug("%x ",dataMessage->pdat[j]);
	}
	debug("\r\n<-----------MQTT end------------>\r\n");*/

	Free_ControlMessage(message);		//释放控制报文结构体内存
	return dataMessage;
}

// 发送MQTT到服务器
ERROTYPE ICACHE_FLASH_ATTR myMQTT_SendtoServer(SessionStr* ss)
{

	DataMessageStr*  ds = myMQTT_CreatControlMessage(myMQTT_CreatMessage(ss));	// 组织发送控制报文数据
	if(ss->erro)return ss->erro;
	ESP8266_SendtoService(ds->pdat,ds->length);								// 则向网络发送数据
	free(ds->pdat);
	free(ds);
	return ss->erro;
}

//释放链表下的主题节点
void ICACHE_FLASH_ATTR FreeSubnodInLink(SessionStr* ss,SingleList* list,void* nod)
{
	subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	free(su->sub);
	free(su);
	SingleList_DeleteNode(list, SingeListGetnode(idNodeStr,nod));// id链表中删除该主题
}
//释放id链表下的publish节点
void ICACHE_FLASH_ATTR FreePubnodInIdLink(SessionStr* ss,void* nod)
{
	pubStr* pu = (pubStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	free(pu->sub);
	free(pu->pub);
	free(pu);
	SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id链表中删除该主题
}

// 接收到服务器的推送,需要释放返回的空间
pubStr* myMQTT_PublishCB(SessionStr* ss,char * pdata, unsigned short len)
{
	if(pdata[0] & PUBLISH == 0) return 0 ;
	pubStr* pu = (pubStr*)malloc(sizeof(pubStr));
	pu->dup = pdata[0] & dup_yes;
	pu->reqQos = pdata[0] & (PUBLISH_QoS1|PUBLISH_QoS2);
	pu->retain = pdata[0] & 0x01;
	u8 lenbyte = 0; //需要的字节数
	u32 length = Change128ToInt(&pdata[1],&lenbyte);//获得剩余长度,需要的字节数
	u16 sublen = (u16)(((u16)pdata[1+lenbyte]<<8)|pdata[2+lenbyte]);

	pu->sub = (u8*)malloc(1 + sublen);
	memcpy(pu->sub,&pdata[3+lenbyte],sublen);						// 保存字符串
	if(pu->reqQos)
	{
		u8 buff[4] = {0};
		pu->pub = (u8*)malloc(1 + length-sublen-2-2);
		memcpy(pu->pub,&pdata[3+lenbyte+sublen + 2],length-sublen-2-2);				// 保存字符串
		// 回复确认
		buff[0] = PUBACK;
		buff[1] = 2;
		buff[2] = pdata[3+lenbyte+sublen];
		buff[3] = pdata[3+lenbyte+sublen + 1];
		ESP8266_SendtoService(buff,4);											// 则向网络发送数据
	}
	else
	{
		pu->pub = (u8*)malloc(1 + length-sublen-2);
		memcpy(pu->pub,&pdata[3+lenbyte+sublen],length-sublen-2);				// 保存字符串
	}
	return pu;
}
