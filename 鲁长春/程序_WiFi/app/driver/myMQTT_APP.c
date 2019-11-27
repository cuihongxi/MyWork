#include "myMQTT_APP.h"
#include "myWifi.h"



// �Ự��CONNECT����
void ICACHE_FLASH_ATTR myMQTTConnect(SessionStr* ss)
{
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	myMQTT_SendtoServer(ss);
}

// ��Ӷ������ⱨ��
void ICACHE_FLASH_ATTR myMQTTAddSubscribe(SessionStr* ss,char* sub,u8 reqQos)
{
	//���ұ��ı�ʶ����������û�иö���
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(CampareString(sub,SingeListGetnode(idNodeStr,nod)->sub.subname))
			{
				debug("id�������Ѵ��ڵ�����\r\n");
				SingleList_MoveEndNode(ss->idList,nod);
				return;
			}
	}
	if(nod == 0)	//˵������һ��������
	{
		debug("����һ��������\r\n");
		idNodeStr* su = (idNodeStr*)malloc(sizeof(idNodeStr));
		su->sub.subname = (u8*)malloc(1 + strlen(sub));
		memcpy(su->sub.subname,sub,strlen(sub));		// �����ַ���
		su->id = ss->id;
		su->sub.reqQos = reqQos;
		SingleList_InsertEnd(ss->idList,su);			// ���ñ�����ӵ������͵�������
	}

}

// �������ⱨ��
void ICACHE_FLASH_ATTR myMQTTSubscribe(SessionStr* ss,char* sub,u8 reqQos)
{
	myMQTTAddSubscribe(ss,sub,reqQos);
	ss->messageType = SUBSCRIBE;
	ss->FixPayload = FixSubscribePayload;
	ss->FixVariableHeader = FixSubscribeVariableHeader;
	myMQTT_SendtoServer(ss);
}

// ȡ�����ı���
void ICACHE_FLASH_ATTR myMQTTUnSubscribe(SessionStr* ss,char* sub)
{
	myMQTTAddSubscribe(ss,sub,0);
	ss->messageType = UNSUBSCRIBE;
	ss->FixPayload = FixUnSubscribePayload;
	ss->FixVariableHeader = FixUnSubscribeVariableHeader;
	myMQTT_SendtoServer(ss);

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
	ss->idList = NewSingleList();
	ss->Subscribe = myMQTTSubscribe;
	ss->Unsub = myMQTTUnSubscribe;
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


//���ĳɹ�,����ID������
void SubscribeSuccessfule(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
			{
				SingleListNode* nodsub = (SingleListNode*)ss->subList;
				while(SingleList_Iterator(&nodsub))		// ��ѯ������������û����ͬ������
				{
					if(CampareString(SingeListGetnode(subStr,nodsub)->subname, SingeListGetnode(idNodeStr,nod)->sub.subname))
					{
						debug("�Ѿ����Ĺ�������\r\n");
						//�ͷſռ�
						free(SingeListGetnode(idNodeStr,nod)->sub.subname);
						SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id������ɾ��������
						return;
					}
				}
				if(nodsub == 0)	// ����һ��������
				{
					debug("����һ��������\r\n");
					subStr* su = (subStr*)malloc(sizeof(subStr));
					su->subname = SingeListGetnode(idNodeStr,nod)->sub.subname;
					su->reqQos = SingeListGetnode(idNodeStr,nod)->sub.reqQos;
					SingleList_InsertEnd(ss->subList,su);								// ���ñ�����ӵ�����������
					SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));	// id������ɾ��������
				}
			}
	}
}

//�ͷű�������ڵ�Ŀռ�
void FreeIDLinkenode(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
			{
				//debug("���ı�ʶ�����ͷſռ�\r\n");
				free(SingeListGetnode(idNodeStr,nod)->sub.subname);
				SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id������ɾ��������
			}
	}
}

// ȡ�����ĳɹ��ص�
void UnSubscribeCallBack(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
		{
			SingleListNode* nodsub = (SingleListNode*)ss->subList;
			while(SingleList_Iterator(&nodsub))		// ��ѯ������������û����ͬ������
			{
				if(CampareString(SingeListGetnode(subStr,nodsub)->subname, SingeListGetnode(idNodeStr,nod)->sub.subname))
				{
					free(SingeListGetnode(subStr,nodsub)->subname);
					SingleList_DeleteNode(ss->subList, SingeListGetnode(subStr,nodsub));// sub������ɾ��������
					break;
				}
			}
			//�ͷſռ�
			free(SingeListGetnode(idNodeStr,nod)->sub.subname);
			SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));// id������ɾ��������
		}
	}
}
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
			if(pdata[4] == 0x80)
				{
					debug("���ı��ģ�%d ʧ�� \r\n",GetU16LittleEnding(&pdata[2]));//����ʧ��
					FreeIDLinkenode(ss,GetU16LittleEnding(&pdata[2]));
				}
			else
				{
					debug("���ı��ģ�%d �ɹ� \r\n",GetU16LittleEnding(&pdata[2]));//����ʧ��,pdata[3]|((u16)pdata[2]<<8)
					SubscribeSuccessfule(ss,GetU16LittleEnding(&pdata[2]));
				}
			break;
		case UNSUBACK:	//ȡ����������ظ�
				debug("ȡ������ID��%d\r\n",GetU16LittleEnding(&pdata[2]));
				UnSubscribeCallBack(ss,GetU16LittleEnding(&pdata[2]));
	}

}
