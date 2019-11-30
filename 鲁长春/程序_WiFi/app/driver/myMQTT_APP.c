/**
 * 2019��11��30��02:02:22
 * ��������������̣�
 * 		����������ʱ������������ӵ����ı�ʶ������idList�С�
 * 		��������ʱ��������ⱨ�ı�ʶ����������û�и����⣬����У������ƶ�������ĩ�ˣ��Ա���������͡�
 * 		�յ��������ظ���������ʧ�ܣ��ӱ��ı�ʶ��������ɾ���ýڵ㣬���ͷſռ䡣�ɹ�ʱ�ӱ��ı�ʶ��������ժ�£�����ӵ����������¡�
 * 	ȡ����������������̣�
 * 		�����������⴦�����û�������������ӵ����ı�ʶ�������С�
 * 		�յ��������ظ������ұ��ı�ʶ����������ͬID�ı��ģ���ȡ������������������������ͬ�������ı��ģ�����������������ڵ����Ϣɾ����
 * 		�������������û���������������ֻɾ�����ı�ʶ�������µĽڵ�
 * 	Publish�������̣�
 * 			Qos0: ���õȼ����浽���������У��ڽ������Ĵ������������� ��ֱ�ӷ�����Ϣ����ɾ���ڵ�
 * 			Qos1���Ƚ�����Ϣ��ӵ����ı�ʶ�������£��ٽ��з��͡��յ��������ظ����ٽ��ýڵ�ɾ����
 * 			Qos2: �ݲ�����
 *
 */
#include "myMQTT_APP.h"
#include "myWifi.h"


void myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len);// �������ظ��ص�����

// �Ự��CONNECT����
void ICACHE_FLASH_ATTR myMQTTConnect(SessionStr* ss)
{
	ss->erro = ERRO_OK;
	ss->messageType = CONNECT;
	ss->FixPayload = FixConnectPayload;
	ss->FixVariableHeader = FixConnectVariableHeader;
	myMQTT_SendtoServer(ss);
}

// ��Ӷ������ⱨ��
void ICACHE_FLASH_ATTR myMQTTAddSubscribe(SessionStr* ss,char* sub,enumQos reqQos)
{
	//���ұ��ı�ʶ����������û�иö���
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(CampareString(sub,((subStr*)(SingeListGetnode(idNodeStr,nod)->mes))->sub))
			{
				debug("id�������Ѵ��ڵ�����\r\n");
				SingleList_MoveEndNode(ss->idList,nod);
				return;
			}
	}
	if(nod == 0)	//˵������һ������Ϣ
	{
		debug("����һ��������\r\n");
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
		memcpy(su->sub,sub,strlen(sub));				// �����ַ���
		su->reqQos = reqQos;
		idnod->id = ss->id;
		idnod->mes = su;
		SingleList_InsertEnd(ss->idList,idnod);			// ���ñ�����ӵ������͵�������
	}

}

// �������ⱨ��
void ICACHE_FLASH_ATTR myMQTTSubscribe(SessionStr* ss,char* sub,enumQos reqQos)
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

// Publish�����������
void ICACHE_FLASH_ATTR myMQTTAddPublish(SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup)
{
	// ������Ϣ�ȼ�����֯����
   if(reqQos & PUBLISH_QoS2)
		{
			debug("����Qos2�ı��ģ���ʱû����ش�����\r\n");
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

		memcpy(pu->sub,sub,strlen(sub));						// �����ַ���
		memcpy(pu->pub,pubdata,strlen(pubdata));				// �����ַ���
		pu->reqQos = reqQos;
		pu->retain = retain;
		pu->dup = dup;
		idnod->id = ss->id;
		idnod->mes = pu;
		SingleList_InsertEnd(ss->idList,idnod);					// ���ñ�����ӵ������͵�������
	}




}
// Publish����
void ICACHE_FLASH_ATTR myMQTTPublish(SessionStr* ss,char* sub,char* pubdata,enumQos reqQos,bool retain,enumdup dup)
{
	myMQTTAddPublish(ss,sub,pubdata,reqQos,retain,dup);
	ss->messageType = PUBLISH;
	ss->FixPayload = FixPublishPayload;
	ss->FixVariableHeader = FixPublishVariableHeader;
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
	ss->ServerCB = myMQTT_ServerReplyCB;
	ss->Publish = myMQTTPublish;
	ss->erro = ERRO_OK;

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
				subStr* su = (subStr*)(SingeListGetnode(idNodeStr,nod)->mes);
				SingleListNode* nodsub = (SingleListNode*)ss->subList;
				while(SingleList_Iterator(&nodsub))		// ��ѯ������������û����ͬ������
				{
					if(CampareString(SingeListGetnode(subStr,nodsub)->sub,su->sub))
					{
						debug("�Ѿ����Ĺ�������,�ͷ�id����ڵ�\r\n");
						FreeSubnodInLink(ss,ss->idList,nod);//�ͷſռ�
						return;
					}
				}
				if(nodsub == 0)	// ����һ��������
				{
					debug("����һ��������\r\n");
					subStr* su = (subStr*)SingeListGetnode(idNodeStr,nod)->mes;
					SingleList_InsertEnd(ss->subList,su);								// ���ñ�����ӵ�����������
					SingleList_DeleteNode(ss->idList, SingeListGetnode(idNodeStr,nod));	// id������ɾ��������
				}
			}
	}
}

//�ͷű�������ڵ�Ŀռ�
void ICACHE_FLASH_ATTR FreeIDLinkenode(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
			{
				//�ͷ�idlinke�����µ�����ڵ�
				 FreeSubnodInLink(ss,ss->idList, nod);
			}
	}
}

// ȡ�����ĳɹ��ص�
void ICACHE_FLASH_ATTR UnSubscribeCallBack(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
		{
			SingleListNode* nodsub = (SingleListNode*)ss->subList;
			while(SingleList_Iterator(&nodsub))		// ��ѯ������������û����ͬ������
			{
				if(CampareString(SingeListGetnode(subStr,nodsub)->sub, ((subStr*)SingeListGetnode(idNodeStr,nod)->mes)->sub))
				{
					SingleList_DeleteNode(ss->subList, SingeListGetnode(subStr,nodsub));// sub������ɾ��������
					break;
				}
			}
			//�ͷſռ�
			FreeSubnodInLink(ss,ss->idList,nod);// id������ɾ��������
		}
	}
}

//publish���Ļظ��յ�,����ID������
void PublishSuccessfule(SessionStr* ss,IDTYPE id)
{
	SingleListNode* nod = (SingleListNode*)ss->idList;
	while(SingleList_Iterator(&nod))
	{
		if(SingeListGetnode(idNodeStr,nod)->id == id)	// ������ͬID
			{
				FreePubnodInIdLink(ss,nod);	// �ͷŽڵ�
			}
	}
}
// �������ظ��ص�����
void ICACHE_FLASH_ATTR myMQTT_ServerReplyCB(SessionStr* ss,char * pdata, unsigned short len)
{
	u8 lenbyte = 0;
	if(len != Change128ToInt((u8*)&pdata[1],&lenbyte) + lenbyte + 1) return;	// ����У�����ݳ��ȶԲ���
	switch(pdata[0]&0xf0){
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
			break;
		case PUBACK:	// pulish ���Ļظ�
				debug("�յ�publis�ظ� ID:%d\r\n",GetU16LittleEnding(&pdata[2]));
				PublishSuccessfule(ss,GetU16LittleEnding(&pdata[2]));
			break;
		case PUBLISH:	// pulish ����
				debug("�յ������������ͣ�\r\n");
				pubStr* pub = myMQTT_PublishCB(ss,pdata,len);
				debug("���⣺%s\r\n",pub->sub);
				debug("���ݣ�%s\r\n",pub->pub);
				free(pub->pub);
				free(pub->sub);
				free(pub);
			break;
	}
}


