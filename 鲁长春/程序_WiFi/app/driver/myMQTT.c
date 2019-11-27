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
	cs->variableHeader.length = 10;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
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
	cs->payload.length = GetStringByteNum(ss->clientId) + GetStringByteNum(ss->usrName) + GetStringByteNum(ss->passWord) +6;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
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
	cs->variableHeader.length = 2;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	cs->variableHeader.pdat[0] = (u8)((ss->id)>>8);
	cs->variableHeader.pdat[1] = (u8)(ss->id);
}



// 填充订阅报文有效载荷
void ICACHE_FLASH_ATTR  FixSubscribePayload(ControlStr* cs,SessionStr* ss)
{

	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	u16 len = GetStringByteNum((const char*)(SingeListGetnode(idNodeStr,nod)->sub.subname));
	cs->payload.length = 3 + len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	Str2ByteSector(SingeListGetnode(idNodeStr,nod)->sub.subname,cs->payload.pdat);
	cs->payload.pdat[cs->payload.length - 1] = SingeListGetnode(idNodeStr,nod)->sub.reqQos;
	ss->id ++;	//报文标识符自增一次
}
/*********************Subscribe报文*****************************/
/*********************UnSubscribe报文*****************************/
// 填充订阅报文可变报头
void ICACHE_FLASH_ATTR  FixUnSubscribeVariableHeader(ControlStr* cs,SessionStr* ss)
{
	cs->variableHeader.length = 2;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	cs->variableHeader.pdat[0] = (u8)((ss->id)>>8);
	cs->variableHeader.pdat[1] = (u8)(ss->id);
}

// 填充取消订阅报文有效载荷
void ICACHE_FLASH_ATTR  FixUnSubscribePayload(ControlStr* cs,SessionStr* ss)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// 取出最后一个链表节点
	u16 len = GetStringByteNum((const char*)(SingeListGetnode(idNodeStr,nod)->sub.subname));
	cs->payload.length = 2 + len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	Str2ByteSector(SingeListGetnode(idNodeStr,nod)->sub.subname,cs->payload.pdat);
	ss->id ++;	//报文标识符自增一次

}
/*********************UnSubscribe报文*****************************/

/**
 * 创建报文
 */
ControlStr* ICACHE_FLASH_ATTR  myMQTT_CreatMessage(SessionStr* ss)
{
	ControlStr* cs = mymalloc(sizeof(ControlStr));
	ss->FixVariableHeader(cs,ss);							// 填充报文可变报头
	ss->FixPayload(cs,ss);									// 填充报文有效载荷
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
	DataMessageStr* dataMessage = mymalloc(sizeof(DataMessageStr));
	u8 i = 0;
	u8 lenbyte = 0; //需要的字节数
	u32 len = Change128ToInt(message->fixHeader.leftNum,&lenbyte);//获得剩余长度,需要的字节数
	dataMessage->length = len + lenbyte + 1;
	dataMessage->pdat = mymalloc(dataMessage->length);
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

/*
	debug("MQTT:\r\n");
	u16 j = 0;
	for( j = 0; j<dataMessage->length;j++)
	{
			debug("%x	",dataMessage->pdat[j]);

	}
	debug(" <-MQTT end------------------------------>\r\n");
*/

	Free_ControlMessage(message);		//释放控制报文结构体内存
	return dataMessage;
}

// 发送MQTT到服务器
void ICACHE_FLASH_ATTR myMQTT_SendtoServer(SessionStr* ss)
{
	DataMessageStr*  ds = myMQTT_CreatControlMessage(myMQTT_CreatMessage(ss));	// 组织发送控制报文数据
	ESP8266_SendtoService(ds->pdat,ds->length);								// 则向网络发送数据
	free(ds->pdat);
	free(ds);
}



