#include "myMQTT.h"
#include "myWifi.h"
#include "mystring.h"

/***
 * ��̬�����ڴ棬����������ڴ�ȫ������
 */

void* ICACHE_FLASH_ATTR ICACHE_FLASH_ATTR  Clearmalloc(u32 num)
{
	u8 *p = (u8*)my_malloc(num);
	u32 i = 0;
	for(i = 0;i<num;i++) p[i] = 0;
	return (void*)p;

}

// �Ͽ�����
void ICACHE_FLASH_ATTR  myMQTT_Disconnect(struct _SessionStr* ss)
{
	u8 discmd[2] = {0xe0,00};
	ss->messageType = DISCONNECT;
	espconn_send(ss->espconn,discmd,2);
}

// ��������PING��
void ICACHE_FLASH_ATTR  myMQTT_Ping(struct _SessionStr* ss)
{
	u8 pincmd[2] = {0xc0,00};
	ss->messageType = PINGREQ;
	espconn_send(ss->espconn,pincmd,2);
}


//ת�����ֶΣ����ַ������ֽڵ���ʽ���浽�����У�������ǰ����ӳ���
u8*  ICACHE_FLASH_ATTR Str2ByteSector(u8* str , u8* pdat)
{
	u32 len = strlen(str);
	pdat[0] = len >> 8;
	pdat[1] = (u8)len;
	return memcpy(&pdat[2],str,len);
}

// ���̶���ͷ
void ICACHE_FLASH_ATTR  FixHeader(ControlStr* cs,myMQTT_ControlType type)
{
	cs->fixHeader.control = type;
	IntTo128(cs->payload.length + cs->variableHeader.length,cs->fixHeader.leftNum);
}
/*********************CONNECT����*****************************/
// ���CONNECT���Ŀɱ䱨ͷ
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

// ������ӱ�����Ч�غ�
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
/*********************CONNECT����*****************************/

/*********************Subscribe����*****************************/
// ��䶩�ı��Ŀɱ䱨ͷ
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

// ��䶩�ı�����Ч�غ�
void ICACHE_FLASH_ATTR  FixSubscribePayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// ȡ�����һ������ڵ�
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
	ss->id ++;	//���ı�ʶ������һ��
}
/*********************Subscribe����*****************************/
/*********************UnSubscribe����*****************************/
// ���ȡ�����ı��Ŀɱ䱨ͷ
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

// ���ȡ�����ı�����Ч�غ�
void ICACHE_FLASH_ATTR  FixUnSubscribePayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// ȡ�����һ������ڵ�
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
	ss->id ++;	//���ı�ʶ������һ��

}
/*********************UnSubscribe����*****************************/
/*********************Publish����*****************************/
// ���Publish���Ŀɱ䱨ͷ,������+���ı�ʶ��
void ICACHE_FLASH_ATTR  FixPublishVariableHeader(ControlStr* cs,SessionStr* ss)
{

	if(ss->erro) return;									// ��ʱ����������ȼ�����Ϣ
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// ȡ�����һ������ڵ�
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
		p[0] = (u8)((ss->id)>>8);	// ��ӱ��ı�ʶ��
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

// ���Publish������Ч�غ�
void ICACHE_FLASH_ATTR  FixPublishPayload(ControlStr* cs,SessionStr* ss)
{
	if(ss->erro) return;
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// ȡ�����һ������ڵ�
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
	ss->id ++;	//���ı�ʶ������һ��
	if(pu->reqQos == 0)	//ɾ���ڵ㣬�ͷſռ�
	{
		FreePubnodInIdLink(ss,nod);
	}
}
/*********************Publish����*****************************/
/**
 * ��������
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
	ss->FixVariableHeader(cs,ss);							// ��䱨�Ŀɱ䱨ͷ
	ss->FixPayload(cs,ss);									// ��䱨����Ч�غ�
	if(ss->erro ) return cs;
	FixHeader(cs,ss->messageType);	 						// ���̶���ͷ
	return cs;
}


/**
 * �ͷſ��Ʊ��Ľṹ���ڴ�
 */

void ICACHE_FLASH_ATTR  Free_ControlMessage(ControlStr* message)
{
	myfree(message->variableHeader.pdat);	// �ͷſɱ䱨ͷ�ṹ��
	myfree(message->payload.pdat);			// �ͷ���Ч�غɽṹ��
	myfree(message);						// �ͷſ��Ʊ��Ľṹ��
}

/***
 * ��֯���Ϳ��Ʊ�������,���ͷſ��Ʊ��Ķ�̬�ڴ�
 * �������ݱ���ͷ
 */

DataMessageStr*  ICACHE_FLASH_ATTR  myMQTT_CreatControlMessage(ControlStr* message)
{
	if(message == 0) return 0;
	DataMessageStr* dataMessage = mymalloc(sizeof(DataMessageStr));
	if(dataMessage == 0) return 0;
	u8 i = 0;
	u8 lenbyte = 0; //��Ҫ���ֽ���
	u32 len = Change128ToInt(message->fixHeader.leftNum,&lenbyte);//���ʣ�೤��,��Ҫ���ֽ���
	dataMessage->length = len + lenbyte + 1;
	dataMessage->pdat = mymalloc(dataMessage->length);
	if(dataMessage->pdat == 0)return 0;
	u8* pdat = dataMessage->pdat;
	*pdat = message->fixHeader.control;	//�����ļ�����

	while(i<4)							//����ʣ�೤��
	{
		pdat ++;
		*pdat = message->fixHeader.leftNum[i];
		if(*pdat & 0x80) i++;
		else break;
	}
	pdat ++;
	//�����ɱ䱨ͷ
	memcpy(pdat,message->variableHeader.pdat,message->variableHeader.length);
	pdat += message->variableHeader.length;
	//������Ч�غ�
	memcpy(pdat,message->payload.pdat,message->payload.length);

/*	debug("\r\nMQTT:\r\n");
	u16 j = 0;
	for( j = 0; j<dataMessage->length;j++)
	{
			debug("%x ",dataMessage->pdat[j]);
	}
	debug("\r\n<-----------MQTT end------------>\r\n");*/

	Free_ControlMessage(message);		//�ͷſ��Ʊ��Ľṹ���ڴ�
	return dataMessage;
}

// ����MQTT��������
ERROTYPE ICACHE_FLASH_ATTR myMQTT_SendtoServer(SessionStr* ss)
{

	DataMessageStr*  ds = myMQTT_CreatControlMessage(myMQTT_CreatMessage(ss));	// ��֯���Ϳ��Ʊ�������
	if(ss->erro)return ss->erro;
	ESP8266_SendtoService(ds->pdat,ds->length);								// �������緢������
	free(ds->pdat);
	free(ds);
	return ss->erro;
}

//�ͷ������µ�����ڵ�
void ICACHE_FLASH_ATTR FreeSubnodInLink(SessionStr* ss,SingleList* list,void* nod)
{
	subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	free(su->sub);
	free(su);
	SingleList_DeleteNode(list, SingeListGetnode(idNodeStr,nod));// id������ɾ��������
}
//�ͷ�id�����µ�publish�ڵ�
void ICACHE_FLASH_ATTR FreePubnodInIdLink(SessionStr* ss,void* nod)
{
	pubStr* pu = (pubStr*)(SingeListGetnode(idNodeStr,nod)->mes);
	free(pu->sub);
	free(pu->pub);
	free(pu);
	SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id������ɾ��������
}

// ���յ�������������,��Ҫ�ͷŷ��صĿռ�
pubStr* myMQTT_PublishCB(SessionStr* ss,char * pdata, unsigned short len)
{
	if(pdata[0] & PUBLISH == 0) return 0 ;
	pubStr* pu = (pubStr*)malloc(sizeof(pubStr));
	pu->dup = pdata[0] & dup_yes;
	pu->reqQos = pdata[0] & (PUBLISH_QoS1|PUBLISH_QoS2);
	pu->retain = pdata[0] & 0x01;
	u8 lenbyte = 0; //��Ҫ���ֽ���
	u32 length = Change128ToInt(&pdata[1],&lenbyte);//���ʣ�೤��,��Ҫ���ֽ���
	u16 sublen = (u16)(((u16)pdata[1+lenbyte]<<8)|pdata[2+lenbyte]);

	pu->sub = (u8*)malloc(1 + sublen);
	memcpy(pu->sub,&pdata[3+lenbyte],sublen);						// �����ַ���
	if(pu->reqQos)
	{
		u8 buff[4] = {0};
		pu->pub = (u8*)malloc(1 + length-sublen-2-2);
		memcpy(pu->pub,&pdata[3+lenbyte+sublen + 2],length-sublen-2-2);				// �����ַ���
		// �ظ�ȷ��
		buff[0] = PUBACK;
		buff[1] = 2;
		buff[2] = pdata[3+lenbyte+sublen];
		buff[3] = pdata[3+lenbyte+sublen + 1];
		ESP8266_SendtoService(buff,4);											// �������緢������
	}
	else
	{
		pu->pub = (u8*)malloc(1 + length-sublen-2);
		memcpy(pu->pub,&pdata[3+lenbyte+sublen],length-sublen-2);				// �����ַ���
	}
	return pu;
}
