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

// ���CONNECT���Ŀɱ䱨ͷ
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

// ������ӱ�����Ч�غ�
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

// ��䶩�ı��Ŀɱ䱨ͷ
void ICACHE_FLASH_ATTR  FixSubscribeVariableHeader(ControlStr* cs,SessionStr* ss)
{
	cs->variableHeader.length = 2;
	cs->variableHeader.pdat = malloc(cs->variableHeader.length);
	cs->variableHeader.pdat[0] = (u8)((ss->subList->num)>>8);
	cs->variableHeader.pdat[1] = (u8)(ss->subList->num);
}



// ��䶩�ı�����Ч�غ�
void ICACHE_FLASH_ATTR  FixSubscribePayload(ControlStr* cs,SessionStr* ss)
{

	SingleListNode* nod = (SingleListNode*)ss->subList;
	while(((SingleListNodeStr*)SingleList_Iterator(&nod))->next);		// ȡ�����һ������ڵ�
	u16 len = GetStringByteNum((const char*)(SingeListGetnode(subStr,nod)->subname));
	cs->payload.length = 3 + len;
	cs->payload.pdat =  (u8*)mymalloc(cs->payload.length);
	Str2ByteSector(SingeListGetnode(subStr,nod)->subname,cs->payload.pdat);
	cs->payload.pdat[cs->payload.length - 1] = SingeListGetnode(subStr,nod)->reqQos;
}

/**
 * ��������
 */
ControlStr* ICACHE_FLASH_ATTR  myMQTT_CreatMessage(SessionStr* ss)
{
	ControlStr* cs = mymalloc(sizeof(ControlStr));
	ss->FixVariableHeader(cs,ss);							// ��䱨�Ŀɱ䱨ͷ
	ss->FixPayload(cs,ss);									// ��䱨����Ч�غ�
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
	DataMessageStr* dataMessage = mymalloc(sizeof(DataMessageStr));
	u8 i = 0;
	u8 lenbyte = 0; //��Ҫ���ֽ���
	u32 len = Change128ToInt(message->fixHeader.leftNum,&lenbyte);//���ʣ�೤��,��Ҫ���ֽ���
	dataMessage->length = len + lenbyte + 1;
	dataMessage->pdat = mymalloc(dataMessage->length);
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

//	debug("MQTT:\r\n");
//	u16 j = 0;
//	for( j = 0; j<dataMessage->length;j++)
//	{
//			debug("%x	",dataMessage->pdat[j]);
//
//	}
//	debug(" <-MQTT end------------------------------>\r\n");

	Free_ControlMessage(message);		//�ͷſ��Ʊ��Ľṹ���ڴ�
	return dataMessage;
}


//�жϷ������ظ����ǲ���֮ǰ�����ı��Ļظ�
// �����棬������֮ǰ�ı��Ļظ�
bool ICACHE_FLASH_ATTR Juge_MessageType(SessionStr* ss,u8 dat)
{
	return ((ss->messageType >> 4) +1  == (dat >>4));
}
// CONNECT���ģ��������ظ��ص�����
void ICACHE_FLASH_ATTR Connack_OkCB(){debug("Connack��ok\n");}
void ICACHE_FLASH_ATTR Connack_ErrorPLCB(){debug("Connack��Error PL\n");}
void ICACHE_FLASH_ATTR Connack_ErrorClientID(){debug("Connack��Error ClientID\n");}
void ICACHE_FLASH_ATTR Connack_ErrorServiceCB(){debug("Connack_Error Service\n");}
void ICACHE_FLASH_ATTR Connack_ErrorUserNameCB(){debug("Connack_Error UserName\n");}
void ICACHE_FLASH_ATTR Connack_ErrorAuthCB(){debug("Connack��Error Auth\n");}

// �������ظ��ص�����
void ICACHE_FLASH_ATTR myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len)
{
	u8 lenbyte = 0;
	if(len != Change128ToInt((u8*)&pdata[1],&lenbyte) + 2) return;	// ����У�����ݳ��ȶԲ���
	if(Juge_MessageType(ss,pdata[0]) == FALSE) return;				// �ж��յ��Ļظ���������
	switch(pdata[0]){
		case CONNACK:
			switch(pdata[3])		// �������ӷ�����
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
			if(pdata[1] == 0) debug("�յ�PING�ظ�...\r\n");
			else debug("PING�ظ�����...\r\n");
			break;
		case SUBACK:	//��������ظ�
			if(pdata[1] == 0x80)	debug("���ı��ģ�%d ʧ��\r\n",pdata[3]|((u16)pdata[2]<<8));//����ʧ��
			else debug("���ı��ģ�%d �ɹ�\r\n",pdata[3]|((u16)pdata[2]<<8));//����ʧ��
			break;
	}

}

// �������ⱨ��
void ICACHE_FLASH_ATTR  myMQTT_Subscrible()
{

}


